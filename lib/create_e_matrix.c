/**
 * This file contains a function that creates the Epsilon Matrix (E-matrix). The E-matrix is 
 * created by referring back to the original input data and using the Epsilon Lookup Table to 
 * map each vector to a spot in the E-matrix. The E-matrix is written to an output file 
 * specified by the user.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_c_vectors.h"
#include "create_e_matrix.h"

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
int create_e_matrix(struct luRow **ptrToLookupTable, int sizeofLookupTable, int numRows, int ***ptrToEMat, FILE *input_file, FILE *output_file, int dir, int hist)
{
    struct luRow *lookupTable = *ptrToLookupTable;
    int **eMat = *ptrToEMat;

    int i;

    // E-matrix row index that the current vector corresponds to
    int currentPos = -1;
    // E-matrix column index that the next vector corresponds to
    int nextPos = -1;

    // Placeholder for a vector read in (uncompressed)
    char vector[hist + 1];
    // The placeholder vector compressed
    int cvec[dir];
    // The placeholder vector as a string
    char cvecAsString[dir + 1];

    // Current compressed vector as a string
    char current[dir + 1];
    // Next compressed vector as a string
    char next[dir + 1];

    int errorFlag = 0;

    //Read a single (uncompressed) vector
    read_uncompressed_vectors(vector, hist, input_file);
    //Initialize the compressed palceholder to all zeros
    for (i = 0; i < dir; i++)
    {
        cvec[i] = 0;
    }
    //Compress the vector
    for (i = 0; i < hist; i++)
    {
        int val = vector[i] - '0';
        cvec[val - 1] += 1;
    }
    //Set current vector to the vector just read in
    for (i = 0; i < dir; i++)
        current[i] = cvec[i] + '0';
    current[dir] = '\0';

    //Read all vectors from the input file
    while (read_uncompressed_vectors(vector, hist, input_file) != -1)
    {
        //Initialize the compressed palceholder to all zeros
        for (i = 0; i < dir; i++)
        {
            cvec[i] = 0;
        }
        // Get compressed vec
        for (i = 0; i < hist; i++)
        {
            int val = vector[i] - '0';
            cvec[val - 1] += 1;
        }
        //Set next vector to the vector just read in
        for (i = 0; i < dir; i++)
            next[i] = cvec[i] + '0';
        next[dir] = '\0';

        //Use the lookup table to find the index that corresponds to
        // the current vector (row index)
        for (i = 0; i < sizeofLookupTable; i++)
        {
            if (strcmp(current, lookupTable[i].vector) == 0)
            {
                currentPos = lookupTable[i].epsilonIdx;
                break;
            }
        }
        //Use the lookup table to find the index that corresponds to
        // the next vector (column index)
        for (int j = 0; j < sizeofLookupTable; j++)
        {
            if (strcmp(next, lookupTable[j].vector) == 0)
            {
                nextPos = lookupTable[j].epsilonIdx;
                break;
            }
        }

        //Increment the correct spot in the e-matrix (using indices just found)
        if ((currentPos != -1) && (nextPos != -1))
        {
            eMat[currentPos][nextPos]++;
        }
        else
        {
            // ERROR: Something went wrong.
            // Could not find a position in the matrix to match the past vector and past+1 vector
            errorFlag = 1;
        }

        //Reset the current and next matrix positions to -1
        // (so that you can tell if a matrix cell is not found)
        currentPos = -1;
        nextPos = -1;
        strcpy(current, next);
    }

    if (errorFlag)
    {
        printf("WARNING: One or more vectors could not be mapped to an epsilon matrix cell.\n");
    }

    // Loop through the lookupTable and find all the unique labels
    // Save them in an array of strings called uniqueLabels
    char **uniqueLabels = malloc(numRows * sizeof(char *));
    int uniqueLabelsCount = 0;
    for (int k = 0; k < sizeofLookupTable; k++)
    {
        if ((k == 0) || (lookupTable[k].epsilonIdx != lookupTable[k - 1].epsilonIdx))
        {
            uniqueLabels[uniqueLabelsCount] = malloc((dir + 1) * sizeof(char));
            strcpy(uniqueLabels[uniqueLabelsCount], lookupTable[k].epsilon);
            uniqueLabelsCount++;
        }
    }
    // Write eMat to output file
    //Displaying array elements
    int row, col, top;
    fprintf(output_file, "Epsilon matrix:\n");
    fprintf(output_file, "            ");
    //Write the column labels at the top of the matrix
    for (top = 0; top < numRows; top++)
    {
        fprintf(output_file, "%12s", uniqueLabels[top]);
        if (top == numRows - 1)
        {
            fprintf(output_file, "\n");
        }
    }

    //Write each row label followed by the corresponding matrix values for that row
    // until the entire matrix is written to the output file
    for (row = 0; row < numRows; row++)
    {
        fprintf(output_file, "%12s", uniqueLabels[row]);
        for (col = 0; col < numRows; col++)
        {
            fprintf(output_file, "%12d", eMat[row][col]);
            if (col == numRows - 1)
            {
                fprintf(output_file, "\n");
            }
        }
    }

    // Free the array that's holding the unique labels
    for (int m = 0; m < numRows; m++)
    {
        free(uniqueLabels[m]);
    }
    free(uniqueLabels);

    fclose(output_file);
    return 1;
}
