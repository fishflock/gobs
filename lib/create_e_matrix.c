#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_c_vectors.h"
#include "create_e_matrix.h"

int create_e_matrix(struct luRow *lookupTable, int sizeofLookupTable, int numRows, int ***ptrToEMat, FILE *input_file, FILE *output_file)
{
    int **eMat = *ptrToEMat;
    printf("\n\nsize of e matrix: %d\n", numRows);
    printf("sizeofLookupTable: %d\n\n", sizeofLookupTable);

    int dir = 4;
    int past[4];
    int future[4];
    int pastEPos = -1;
    int futureEPos = -1;

    while (read_cvec(past, future, input_file) != -1)
    {
        char past_label[dir + 1];
        for (int j = 0; j < dir; j++)
            past_label[j] = past[j] + '0';
        past_label[dir] = '\0';
        //printf("\n%s : ", past_label);

        // For each entry in the lookup table
        for (int j = 0; j < sizeofLookupTable; j++)
        {
            char currPastVec[dir + 1];
            for (int i = 0; i < dir; i++)
                currPastVec[i] = lookupTable[j].vector[i] + '0';
            currPastVec[dir] = '\0';
            //printf("%s, ", currPastVec);

            if (strcmp(past_label, currPastVec) == 0)
            {
                pastEPos = get_epsilon_pos(lookupTable[j].epsilon, lookupTable, sizeofLookupTable);
                break;
            }
        }

        char future_label[dir + 1];
        for (int j = 0; j < dir; j++)
            future_label[j] = future[j] + '0';
        future_label[dir] = '\0';
        //printf("\n%s : ", future_label);

        // For each entry in the lookup table
        for (int j = 0; j < sizeofLookupTable; j++)
        {
            char currFutureVec[dir + 1];
            for (int i = 0; i < dir; i++)
                currFutureVec[i] = lookupTable[j].vector[i] + '0';
            currFutureVec[dir] = '\0';
            //printf("%s, ", currFutureVec);

            if (strcmp(future_label, currFutureVec) == 0)
            {
                futureEPos = get_epsilon_pos(lookupTable[j].epsilon, lookupTable, sizeofLookupTable);
                ;
                break;
            }
        }

        if ((pastEPos != -1) && (futureEPos != -1))
        {
            eMat[pastEPos][futureEPos]++;
        }
        else
        {
            printf("Could not find a position in the matrix to match: \n");
            printf("past vector: ");
            for (int i = 0; i < 4; i++)
            {
                printf("%d", past[i]);
                if (i == 3)
                {
                    printf("\n");
                }
            }
            printf("future vector: ");
            for (int i = 0; i < 4; i++)
            {
                printf("%d", future[i]);
                if (i == 3)
                {
                    printf("\n");
                }
            }
        }
    }

    // Write eMat to output file
    // Open output file
    int temp_buff_size = 128;
    char filename[temp_buff_size];
    strcpy(filename, "output");
    strcat(filename, ".txt");
    output_file = fopen(filename, "w");

    //Displaying array elements
    int row, col, top;
    fprintf(output_file, "Epsilon matrix:\n");
    fprintf(output_file, "      ");
    for (top = 0; top < numRows; top++)
    {
        fprintf(output_file, "%s ", lookupTable[top].epsilon);
        if (top == numRows - 1)
        {
            fprintf(output_file, "\n");
        }
    }
    for (row = 0; row < numRows; row++)
    {
        fprintf(output_file, "%s ", lookupTable[row].epsilon);
        for (col = 0; col < numRows; col++)
        {
            fprintf(output_file, "  %d   ", eMat[row][col]);
            if (col == numRows - 1)
            {
                fprintf(output_file, "\n");
            }
        }
    }
    fclose(output_file);
    return 1;
}

int get_epsilon_pos(char *epString, struct luRow *lookupTable, int sizeofLookupTable)
{
    for (int m = 0; m < sizeofLookupTable; m++)
    {
        if (!strcmp(epString, lookupTable[m].epsilon))
        {
            return m;
        }
    }
    return -1;
}
