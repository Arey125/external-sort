#include <stdio.h>
#include <stdlib.h>
#include "merge.h"

int main(int argc, char const *argv[]) {
    if (argc < 1) {
        printf("Use %s [FILENAMES...]\n", argv[0]);
        return -1;
    }

    FILE **files = malloc(sizeof(FILE *) * (argc - 1));

    for (int i = 1; i < argc; i++) {
        files[i - 1] = fopen(argv[i], "r");
    }

    FILE *out = fopen("out.txt", "w");

    merge(out, argv + 1, argc - 1);

    for (int i = 0; i < argc - 1; i++) {
        fclose(files[i]);
    }

    fclose(out);
    return 0;
}
