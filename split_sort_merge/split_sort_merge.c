#include "split_sort_merge.h"

void split_sort_merge(const char *input_file, const char *output_file, long long int size) {
    char **filenames;
    int count = split(input_file, &filenames, size);

    for (int i = 0; i < size; i++) {
        printf("file %s\n", filenames[i]);
        FILE *file = fopen(filenames[i], "r+w");
        int fd = fileno(file);
        sort_file(fd);
        fclose(file);
    }

    FILE *output = fopen(output_file, "w");

    merge(output, filenames, count);
}
