#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_LINE_SIZE 256
#define BUF_SIZE 65536

int get_line1(FILE *f, char *buf) {
    return fscanf(f, "%s", buf) != EOF;
}

int split(const char *filename, char ***filenames_ptr, int size) {
    FILE *file = fopen(filename, "r");

    setvbuf(file, NULL, _IOFBF, BUF_SIZE);

    if (file == NULL) {
        perror("open");
        return 0;
    }

    int fd = fileno(file);

    long long int f_size;
    struct stat s_fstat;
    if (fstat(fd, &s_fstat) != 0) {
        perror("fstat");
        return 0;
    }

    f_size = s_fstat.st_size;

    long long int count = (f_size + size - 1) / size + 1;
    char **filenames = malloc(count * sizeof(char *));

    *filenames_ptr = filenames;

    char line_buf[MAX_LINE_SIZE];

    int file_size = 0;
    int has_next = get_line1(file, line_buf);

    for (int i = 0; i < count; i++) {
        filenames[i] = malloc(256 * sizeof(char));
        sprintf(filenames[i], "./tmp/%s.part%d.txt", filename, i);

        FILE *new_file = fopen(filenames[i], "w");
        setvbuf(file, NULL, _IOFBF, BUF_SIZE);

        if (new_file == NULL) {
            perror("open");
            return 0;
        }

        int file_size = 0;
        while (1) {
            if (!has_next) {
                fclose(new_file);
                count = i + 1;
                break;
            }

            if (file_size < size) {
                fprintf(new_file, "%s", line_buf);
                file_size += strlen(line_buf);
                has_next = get_line1(file, line_buf);
            }

            if (file_size >= size) {
                fclose(new_file);
                break;
            }

            if (has_next) {
                fprintf(new_file, "\n");
            }
        }
    }

    fclose(file);

    return count;
}
