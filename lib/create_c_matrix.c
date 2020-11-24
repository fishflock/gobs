/**
 * This file contains functions that can be used to create the co-occurrence matrix
 * (C-matrix). The co-occurence matrix is produced by methodically compressing the 
 * data read from the input file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_c_vectors.h"
#include "create_c_matrix.h"

/**
 * This function uses data read from the input file to create the C-matrix.
 * 
 * @param dir: Number of directions used in data collection (user-specified)
 * @param hist: Number of integers to be used in a single vector (user-specified)
 * @param ptrToLabels: Pointer to the array of strings that are the labels for the C-matrix
 *                      (memory should be allocated to the correct size, and should only 
 *                       be initialized to zeros--no real data in it yet)
 * @param ptrToMatrix: Pointer to the C-matrix
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
                    FILE *file, int *numVecsRecorded)
{
    char **labels = *ptrToLabels;
    double **matrix = *ptrToMatrix;
    char past[dir + 1];
    char future[dir + 1];

    int last_label_pos = 0;
    int i;
    *numVecsRecorded = 0;
    while (read_c_vectors(past, future, hist, dir, file) != -1)
    {
        int past_pos = -1;
        for (int i = 0; i < last_label_pos + 1; i++)
        {
            if (strcmp(past, labels[i]) == 0)
            {
                //printf("Found a PAST match\n");
                past_pos = i;
            }
        }
        if (past_pos == -1)
        {
            //printf("Couldn't find a PAST match. Adding a new label!\n");
            strcpy(labels[last_label_pos], past);
            past_pos = last_label_pos;
            last_label_pos++;
        }

        int future_pos = -1;
        for (int i = 0; i < last_label_pos + 1; i++)
        {
            if (strcmp(future, labels[i]) == 0)
            {
                //printf("Found a FUTURE match\n");
                future_pos = i;
            }
        }
        if (future_pos == -1)
        {
            //printf("Couldn't find a FUTURE match. Adding a new label!\n");
            strcpy(labels[last_label_pos], future);
            future_pos = last_label_pos;
            last_label_pos++;
        }

        matrix[past_pos][future_pos]++;
        *numVecsRecorded = *numVecsRecorded + 1;
    }
    printf("\n\n NUMBER OF VECTORS RECORDED: %d\n", *numVecsRecorded);
    fseek(file, 0, SEEK_SET);
    return last_label_pos;
}

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
int normalize_c_matrix(int realSize, double ***ptrToMatrix, int numVecsRecorded, int historyLength)
{
    int denominator = numVecsRecorded - 2 * historyLength;
    printf("Normalizing C matrix by a factor of: %d\n", denominator);
    double **matrix = *ptrToMatrix;
    int i, j;
    int matrixSum = 0;
    for (i = 0; i < realSize; i++)
    {
        for (j = 0; j < realSize; j++)
        {
            matrix[i][j] = matrix[i][j] / denominator;
        }
    }
    return 1;
}