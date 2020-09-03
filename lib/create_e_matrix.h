#ifndef CREATE_E_MATRIX_H
#define CREATE_E_MATRIX_H
#include <stdio.h>
#include "structs.h"

int get_epsilon_pos(char *epString, struct luRow *lookupTable, int sizeofLookupTable);

int create_e_matrix(struct luRow *lookupTable, int sizeofLookupTable, int numRows, int ***ptrToEMat, FILE *input_file, FILE *output_file);
#endif