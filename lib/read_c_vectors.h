#ifndef READ_C_VECTOR_H
#define READ_C_VECTOR_H
#include <stdio.h>

int read_c_vectors(int * past_c_vector, int * future_c_vector, 
                    int length, int directions, FILE * file);
int read_cvec(int * past_c_vector, int * future_c_vector, FILE * file);

#endif

