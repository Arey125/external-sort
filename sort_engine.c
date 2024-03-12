#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

char* singlethread_sort(char* buf, long long int f_size){

    long long int count = 1;
    for (int i = 0; i < f_size; i++) {
        if (buf[i] == '\n') {
            count++;
        }
    }
    printf("Counts in file: %lld\n", count);
    
    char **lines = malloc(count * sizeof(char*));
    int start = 0;
    char* token;

    token = strtok(buf, "\n");
    while (start != count)
    {
        lines[start] = token;
        start += 1;
        token = strtok(NULL, "\n");
    }
    
    printf("qsort starting...\n");
    qsort(lines, count, sizeof(char *), compare);
    printf("qsort finished...\n");
    char* tmp_buf = malloc(f_size);
    char* cursor = tmp_buf;
    int len = 0;
    for (int i = 0; i < count; i++)
    {
        int str_len = strlen(lines[i]) + 1;
        strncpy(cursor, lines[i], str_len);
        cursor[str_len - 1] = '\n';
        //printf("%s    len == %d\n", lines[i], str_len);
        cursor += str_len;
    }
    printf("cursor  =  %d\n", len);

    return tmp_buf;
}