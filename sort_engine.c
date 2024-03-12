#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>

struct offsets
{
    long long int filesize;
    long long int old_offset;
};
 
int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

char *singlethread_sort(char *buf, long long int f_size) {

    long long int count = 1;
    for (int i = 0; i < f_size; i++) {
        if (buf[i] == '\n') {
            count++;
        }
    }
    printf("Counts in file: %lld\n", count);
  
    char **lines = malloc(count * sizeof(char *));
    int start = 0;
    char *token;
  
    token = strtok(buf, "\n");
    while (start != count) {
        lines[start] = token;
        start += 1;
        token = strtok(NULL, "\n");
    }
  
    printf("qsort starting...\n");
    qsort(lines, count, sizeof(char *), compare);
    printf("qsort finished...\n");
    char *tmp_buf = malloc(f_size);
    char *cursor = tmp_buf;
    int len = 0;
    for (int i = 0; i < count; i++) {
        int str_len = strlen(lines[i]) + 1;
        strncpy(cursor, lines[i], str_len);
        cursor[str_len - 1] = '\n';
        // printf("%s    len == %d\n", lines[i], str_len);
        cursor += str_len;
    }
    printf("cursor  =  %d\n", len);
  
    return tmp_buf;
}

void sort_file(int fd) {
    struct stat s_fstat; //sys ctruct for file info
    struct offsets s_offset = {0, 0}; // struct for multithreads sorts, manual wrote

    if (fstat(fd, &s_fstat) != 0) {
        perror("fstat() error");
        exit(1);
    }

    int f_size = s_fstat.st_size;
    printf("File size: %lld bytes, [%lld MB]\n", f_size, f_size / (1024 * 1024));

    char *buf = mmap(NULL, f_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                     s_offset.old_offset);
    if (buf == MAP_FAILED) {
        perror("(1)mmap() error");
        exit(1);
    }

    char *buf_copy; //= malloc(f_size);

    buf_copy = singlethread_sort(buf, f_size);
    memcpy(buf, buf_copy, f_size) ==
        NULL; // WARNING!!! Unsave function! At-ta-ta!!

    if (msync(buf, f_size, MS_ASYNC) != 0) {
        perror("(1)msync() error");
        exit(1);
    }

    munmap(buf, f_size);
    free(buf_copy);
}
