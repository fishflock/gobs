#ifndef READ_C_VECTOR_H
#define READ_C_VECTOR_H
#include <stdio.h>

int read_c_vectors(char *past_c_vector, char *future_c_vector,
                   int length, int directions, FILE *file);
int read_cvec(char *past_c_vector, char *future_c_vector, FILE *file);
int read_uncompressed_vectors(char *vector,
                              int len, FILE *file);

#endif
