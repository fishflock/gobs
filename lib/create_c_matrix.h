#ifndef CREATE_C_MATRIX_H
#define CREATE_C_MATRIX_H
#include <stdio.h>

/**
 * This function used data read from the input file to create the C-matrix.
 * 
 * @param dir: Number of directions used in data collection (user-specified)
 * @param hist: Number of integers to be used in a single vector (user-specified)
 * @param ptrToLabels: Pointer to the array of strings that are the labels for the C-matrix
 *                      (memory should be allocated to the correct size, and should only 
 *                       be initialized to zeros--no real data in it yet)
 * @param file: Input file to read data from
 * @param numVecsRecorded: A pointer to an int containing the total number of vectors recorded
 * 
 * @return: (int) Number of rows/columns in C-matrix
 */
int create_c_matrix(int dir, int hist,
                    char ***ptrToLabels,
                    double ***ptrToMatrix,
                    FILE *file, int *numVecsRecorded);

/**
 * This function normalizes the C-matrix by a factor of the total number of vectors 
 * recorded MINUS 2 times the length of a single vector. This function may be useful in 
 * the case of very large input files.
 * 
 * @param realSize: Number of rows/columns in the C-matrix
 * @param ptrToMatrix: Pointer to the C-matrix
 * @param numVecsRecorded: The total number of vectors recorded
 * @param historyLength: Number of integers to be used in a single vector (user-specified)
 */
int normalize_c_matrix(int realSize,
                       double ***ptrToMatrix,
                       int numVecsRecorded,
                       int historyLength);

#endif