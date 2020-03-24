#ifndef CREATE_E_LOOKUP_TABLE_H
#define CREATE_E_LOOKUP_TABLE_H
#include <stdio.h>
#include "structs.h"

int create_e_table(struct luRow *lookupTable, int dir, int size, int matrix[size][size], char labels[size][dir + 1], double ALPHA);
int fill_e_table(int *flags, struct luRow *lookupTable, int dir, int size, int matrix[size][size], char labels[size][dir + 1], double ALPHA);
int compare_rows(int *row1, int *row2, int size, double *total);

#endif