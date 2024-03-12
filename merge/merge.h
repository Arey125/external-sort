#include <stdio.h>

void merge_sorted_files(FILE *output, FILE *l, FILE *r);

void merge(FILE *output, FILE **files, int count);
