#include <stdio.h>
#include <stdlib.h>
#include "split.h"

int main(int argc, char const *argv[]) {
    if (argc < 1) {
        printf("Use %s [FILENAME]\n", argv[0]);
        return -1;
    }
    printf("Spliting %s\n", argv[1]);

    char **filenames;
    int count = split(argv[1], &filenames, 10 * 1000 * 1000);
    printf("Split %d files\n", count);

    for (int i = 0; i < count; i++) {
        printf("File %d: %s\n", i, filenames[i]);
    }

    for (int i = 0; i < count; i++) {
        free(filenames[i]);
    }
    free(filenames);

    return 0;
}
