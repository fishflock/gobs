#ifndef CREATE_C_MATRIX_H
#define CREATE_C_MATRIX_H
#include <stdio.h>

int create_c_matrix(int len, int dir,
                    char labels[816][dir + 1],
                    int matrix[816][816], FILE *file);

#endif