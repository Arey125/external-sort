#include <string.h>

#include "merge.h"

#define BUF_SIZE 65536
#define MAX_LINE_SIZE 256

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
        if (has_next_l && (!has_next_r || strcmp(line_l, line_r) < 0)) {
            fprintf(output, "%s", line_l);
            has_next_l = get_line(l, line_l);
            if (has_next_l || has_next_r) {
                fprintf(output, "\n");
            }
            continue;
        }
        fprintf(output, "%s", line_r);
        has_next_r = get_line(r, line_r);
        if (has_next_l || has_next_r) {
            fprintf(output, "\n");
        }
    }
}

void copy_file(FILE* output, FILE* input) {
    setvbuf(output, NULL, _IOFBF, BUF_SIZE);
    setvbuf(input, NULL, _IOFBF, BUF_SIZE);

    while (1) {
        int c = fgetc(input);
        if (c == EOF) {
            break;
        }
        fputc(c, output);
    }
}

void merge(FILE *output, const char **files, int count) {
    if (count == 1) {
        FILE *file = fopen(files[0], "r");
        copy_file(output, file);
        return;
    }
    int half = count / 2;

    static int file_ind = 0;

    char filename_l[MAX_LINE_SIZE];
    char filename_r[MAX_LINE_SIZE];
    sprintf(filename_l, "./tmp/l_%d.txt", file_ind);
    sprintf(filename_r, "./tmp/r_%d.txt", file_ind);
    file_ind++;

    FILE *l = fopen(filename_l, "w");
    FILE *r = fopen(filename_r, "w");

    merge(l, files, half);
    merge(r, files + half, count - half);
    fclose(l);
    fclose(r);

    l = fopen(filename_l, "r");
    r = fopen(filename_r, "r");
    merge_sorted_files(output, l, r);

    fclose(l);
    fclose(r);
}
