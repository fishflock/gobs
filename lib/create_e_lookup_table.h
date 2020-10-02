#ifndef CREATE_E_LOOKUP_TABLE_H
#define CREATE_E_LOOKUP_TABLE_H
#include <stdio.h>
#include "structs.h"

int create_e_table(struct luRow **lookupTable, int dir, int realSize, double ***ptrToMatrix, char ***ptrToLabels, double ALPHA);
int fill_e_table(int *flags, struct luRow **lookupTable, int dir, int realSize, double ***ptrToMatrix, char ***ptrToLabels, double ALPHA);
int compare_rows(double *row1, double *row2, int realSize, double *total);
int reduce_noise(int realSize, double ***ptrToMatrix);

#endif