#include <stdio.h>

#include "split_sort_merge.h"

int main(int argc, char const *argv[]) {
    if (argc < 1) {
        printf("Use %s [FILENAME]\n", argv[0]);
        return -1;
    }

    split_sort_merge(argv[1], "out.txt", 1000000);
    return 0;
}
