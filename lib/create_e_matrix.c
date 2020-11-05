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
                currentPos = get_epsilon_pos(lookupTable[i].epsilon, &lookupTable, sizeofLookupTable);
                //printf("currentPos: %d\n", currentPos);
                break;
            }
        }
        // For each entry in the lookup table
        for (int j = 0; j < sizeofLookupTable; j++)
        {
            if (strcmp(next, lookupTable[j].vector) == 0)
            {
                nextPos = get_epsilon_pos(lookupTable[j].epsilon, &lookupTable, sizeofLookupTable);
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
            printf("Could not find a position in the matrix to match: \n");
            printf("Past vector: %s\n", current);
            printf("Past+1 vector: %s\n", next);
        }

        strcpy(current, next);
    }

    // Loop through the lookupTable and find all the unique labels
    // Save them in an array of strings called uniqueLabels
    char **uniqueLabels = malloc(numRows * sizeof(char *));
    int uniqueLabelsCount = 0;
    char currentEpsilon[dir + 1];
    strcpy(currentEpsilon, lookupTable[0].epsilon);
    for (int k = 0; k < sizeofLookupTable; k++)
    {
        if ((k == 0) || (strcmp(currentEpsilon, lookupTable[k].epsilon) != 0))
        {
            uniqueLabels[uniqueLabelsCount] = malloc((dir + 1) * sizeof(char));
            strcpy(uniqueLabels[uniqueLabelsCount], lookupTable[k].epsilon);
            strcpy(currentEpsilon, lookupTable[k].epsilon);
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

int get_epsilon_pos(char *epString, struct luRow **ptrToLookupTable, int sizeofLookupTable)
{
    struct luRow *lookupTable = *ptrToLookupTable;
    int unique = 0;
    for (int m = 0; m < sizeofLookupTable; m++)
    {
        if (m > 0)
        {

            if (strcmp(lookupTable[m - 1].epsilon, lookupTable[m].epsilon) == 1)
            {
                // printf("1: lookupTable[m - 1].epsilon ---> %s\n", lookupTable[m - 1].epsilon);
                // printf("2: lookupTable[m].epsilon ---> %s\n", lookupTable[m].epsilon);
                unique++;
            }
        }

        if (!strcmp(epString, lookupTable[m].epsilon))
        {
            return unique;
        }
    }
    return -1;
}
