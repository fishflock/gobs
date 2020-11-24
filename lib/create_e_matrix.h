#ifndef CREATE_E_MATRIX_H
#define CREATE_E_MATRIX_H
#include <stdio.h>
#include "structs.h"

/**
 * This function creates the E-matrix and writes it to the output file specified by the user.
 * 
 * @param ptrToLookupTable: Pointer to the array of luRow structs to represent rows of the 
 *                          lookup table 
 * @param sizeofLookupTable: Number of rows in the Epsilon Lookup Table (this is equivalent to 
 *                              the number of rows/columns in the C-matrix)
 * @param numRows: Number of rows in the E-matrix
 * @param ptrToEMat: Pointer to the E-matrix
 *                   (memory should be allocated to the correct size, and should only 
 *                    be initialized to zeros--no real data in it yet)
 * @param input_file: The input file specified by user
 * @param output_file: The output file specified by user
 * @param dir: Number of directions used in data collection (user-specified)
 * @param hist: Number of integers to be used in a single vector (user-specified)
 */
int create_e_matrix(struct luRow **ptrToLookupTable, int sizeofLookupTable, int numRows, int ***ptrToEMat, FILE *input_file, FILE *output_file, int dir, int hist);
#endif