#include <string.h>

#include "merge.h"

#define BUF_SIZE 1024
#define MAX_LINE_SIZE 128

int get_line(FILE* f, char* buf) {
    return fscanf(f, "%s", buf) != EOF;
}

void merge_sorted_files(FILE *output, FILE *l, FILE *r) {
    char buf[1024];

    setvbuf(l, NULL, _IOFBF, BUF_SIZE);
    setvbuf(r, NULL, _IOFBF, BUF_SIZE);
    setvbuf(output, NULL, _IOFBF, BUF_SIZE);

    char line_l[MAX_LINE_SIZE];
    char line_r[MAX_LINE_SIZE];

    int has_next_l = get_line(l, line_l);
    int has_next_r = get_line(r, line_r);

    while (has_next_l || has_next_r) {
        if (has_next_l && (strcmp(line_l, line_r) < 0 || !has_next_r)) {
            fprintf(output, "%s\n", line_l);
            has_next_l = get_line(l, line_l);
            continue;
        }
        fprintf(output, "%s\n", line_r);
        has_next_r = get_line(r, line_r);
    }
}
