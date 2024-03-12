#include <stdio.h>
#include "merge.h"

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        printf("Use %s [FILENAME] [FILENAME]\n", argv[0]);
        return -1;
    }

    FILE *l = fopen(argv[1], "r");
    FILE *r = fopen(argv[2], "r");
    FILE *out = fopen("out.txt", "w");

    merge_sorted_files(out, l, r);
    fclose(l);
    fclose(r);
    fclose(out);
    return 0;
}
