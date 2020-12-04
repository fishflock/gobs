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

    //Read the input file one past and future vector at a time (until the end of file)
    while (read_c_vectors(past, future, hist, dir, file) != -1)
    {
        //See if the current past vector already exists as a row in the C matrix.
        int past_pos = -1;
        for (int i = 0; i < last_label_pos + 1; i++)
        {
            if (strcmp(past, labels[i]) == 0)
            {
                //The past vector already exists as a row in the matrix.
                // Set past vector's position to the corresponding index
                past_pos = i;
            }
        }
        if (past_pos == -1)
        {
            //The past vector does NOT exist in the matrix.
            // Add it as a row in the matrix
            strcpy(labels[last_label_pos], past);
            past_pos = last_label_pos;
            last_label_pos++;
        }

        //See if the current future vector already exists as a row in the C matrix.
        int future_pos = -1;
        for (int i = 0; i < last_label_pos + 1; i++)
        {
            if (strcmp(future, labels[i]) == 0)
            {
                //The future vector already exists as a row in the matrix.
                // Set future vector's position to the corresponding index
                future_pos = i;
            }
        }
        if (future_pos == -1)
        {
            //The future vector does NOT exist in the matrix.
            // Add it as a row in the matrix
            strcpy(labels[last_label_pos], future);
            future_pos = last_label_pos;
            last_label_pos++;
        }

        //Increment the position in the matrix that corresponds to the
        // past and future indices
        matrix[past_pos][future_pos]++;

        //Keep track of the number of vectors recorded for later use
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
    //Find the value to normalize the matrix by
    int denominator = numVecsRecorded - 2 * historyLength;
    printf("Normalizing C matrix by a factor of: %d\n", denominator);
    double **matrix = *ptrToMatrix;

    //Loop through and divide each matrix value by the denominator found above
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