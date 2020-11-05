#ifndef CREATE_E_MATRIX_H
#define CREATE_E_MATRIX_H
#include <stdio.h>
#include "structs.h"

int get_epsilon_pos(char *epString, struct luRow **ptrToLookupTable, int sizeofLookupTable);

int create_e_matrix(struct luRow **ptrToLookupTable, int sizeofLookupTable, int numRows, int ***ptrToEMat, FILE *input_file, FILE *output_file, int dir, int hist);
#endif