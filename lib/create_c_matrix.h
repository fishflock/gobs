#ifndef CREATE_C_MATRIX_H
#define CREATE_C_MATRIX_H
#include <stdio.h>

int create_c_matrix(int dir,
                    char ***ptrToLabels,
                    double ***ptrToMatrix, FILE *file, int *numVecsRecorded);

int normalize_c_matrix(int realSize,
                       double ***ptrToMatrix,
                       int numVecsRecorded,
                       int historyLength);

#endif