#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_c_vectors.h"
#include "create_e_matrix.h"

int create_e_matrix(struct luRow **ptrToLookupTable, int sizeofLookupTable, int numRows, int ***ptrToEMat, FILE *input_file, FILE *output_file, int dir, int hist)
{
    struct luRow *lookupTable = *ptrToLookupTable;
    int **eMat = *ptrToEMat;

    int i;
    int currentPos = -1;
    int nextPos = -1;

    char vector[hist + 1];
    int cvec[dir];
    char cvecAsString[dir + 1];
    char current[dir + 1];
    char next[dir + 1];
    int errorFlag = 0;

    read_uncompressed_vectors(vector, hist, input_file);

    for (i = 0; i < dir; i++)
    {
        cvec[i] = 0;
    }
    //printf("%s -> ", vector);
    // Get compressed vec
    for (i = 0; i < hist; i++)
    {
        int val = vector[i] - '0';
        cvec[val - 1] += 1;
    }
    // for (i = 0; i < dir; i++)
    // {
    //     printf("%d", cvec[i]);
    // }
    //printf("\n");
    for (i = 0; i < dir; i++)
        current[i] = cvec[i] + '0';
    current[dir] = '\0';

    while (read_uncompressed_vectors(vector, hist, input_file) != -1)
    {
        for (i = 0; i < dir; i++)
        {
            cvec[i] = 0;
        }
        //printf("%s -> ", vector);
        // Get compressed vec
        for (i = 0; i < hist; i++)
        {
            int val = vector[i] - '0';
            cvec[val - 1] += 1;
        }
        // for (i = 0; i < dir; i++)
        // {
        //     printf("%d", cvec[i]);
        // }
        // printf("\n");
        for (i = 0; i < dir; i++)
            next[i] = cvec[i] + '0';
        next[dir] = '\0';

        for (i = 0; i < sizeofLookupTable; i++)
        {
            if (strcmp(current, lookupTable[i].vector) == 0)
            {
                currentPos = lookupTable[i].epsilonIdx;
                //printf("currentPos: %d\n", currentPos);
                break;
            }
        }
        // For each entry in the lookup table
        for (int j = 0; j < sizeofLookupTable; j++)
        {
            if (strcmp(next, lookupTable[j].vector) == 0)
            {
                nextPos = lookupTable[j].epsilonIdx;
                //printf("nextPos: %d\n", nextPos);
                break;
            }
        }

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
    for (top = 0; top < numRows; top++)
    {
        fprintf(output_file, "%12s", uniqueLabels[top]);
        if (top == numRows - 1)
        {
            fprintf(output_file, "\n");
        }
    }

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
