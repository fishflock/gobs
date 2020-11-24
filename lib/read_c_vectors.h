#ifndef READ_C_VECTOR_H
#define READ_C_VECTOR_H
#include <stdio.h>

/**
 * This function reads a past and future vector from the input file and COMPRESSES them. 
 * This function should be called repeatedly to loop through entire input file.
 * 
 * Notes:
 *  Assumes directions start at 1, unlike array positions (start at 0).
 *  Assumes the arrays that the pointers point to are initialized to
 *  the size of the number of directions (dir).
 * 
 * @param past: Compression vector of past history vector, size dir
 * @param future: Compression vector of future history vector, size dir
 * @param len: Size of history vector (past and future)
 * @param dir: Number of possible directions
 * @param file: File to be read from
 *
 * @return: -1 on failure; 1 on success
 */
int read_c_vectors(char *past_c_vector, char *future_c_vector,
                   int length, int directions, FILE *file);

/**
 * This function reads a vector from the input file. This function should be 
 * called repeatedly to loop through entire input file.
 * 
 * Notes:
 *  Assumes directions start at 1, unlike array positions (start at 0).
 *  Assumes the arrays that the pointers point to are initialized to
 *  the size of the number of directions (dir).
 * 
 * @param vector: Vector of past history vector, size len
 * @param len: Size of history vector (past and future)
 * @param file: File to be read from
 *
 * @return: -1 on failure; 1 on success
 */
int read_uncompressed_vectors(char *vector, int len, FILE *file);

#endif
