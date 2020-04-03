#ifndef CREATE_C_MATRIX_H
#define CREATE_C_MATRIX_H
#include <stdio.h>

int create_c_matrix(int len, int dir, int size,
                    char labels[size][dir + 1],
                    int matrix[size][size], FILE *file);
int create_c_matrix_default(int size, char labels[size][5],
                            int matrix[size][size], FILE *file);

unsigned long long int fact(int n);
int n_choose_r(int n, int r);
int max_vectors(int hist, int dir, FILE *file);

#endif