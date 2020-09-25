#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "create_e_lookup_table.h"

int create_e_table(struct luRow **ptrToLookupTable, int dir, int realSize, int ***ptrToMatrix, char ***ptrToLabels, double ALPHA)
{
    struct luRow *lookupTable = *ptrToLookupTable;
    char **labels = *ptrToLabels;
    int **matrix = *ptrToMatrix;

    // Create flags array (each flag corresponds to a row)
    int flags[realSize];
    flags[0] = 1;
    flags[1] = 1;
    for (int i = 2; i < realSize; i++)
    {
        flags[i] = 0;
    }

    // Add first row of C matrix to the epsilon lookup table
    strcpy(lookupTable[0].epsilon, labels[1]);

    // Add the corresponding vector of first row to the epsilon table
    strcpy(lookupTable[0].vector, labels[1]);

    int fillTab = fill_e_table(flags, &lookupTable, dir, realSize, &matrix, &labels, ALPHA);
    return fillTab;
}

int fill_e_table(int *flags, struct luRow **ptrToLookupTable, int dir, int realSize, int ***ptrToMatrix, char ***ptrToLabels, double ALPHA)
{
    struct luRow *lookupTable = *ptrToLookupTable;
    char **labels = *ptrToLabels;
    int **matrix = *ptrToMatrix;

    int lastEl = 1;
    int currentLead = 1;
    double total = 0;
    int h, i, j;
    // Loop through every row in matrix
    for (h = 2; h < realSize; h++)
    {
        //printf("...labels[currentLead]: %s\n", labels[currentLead]);
        // IF it hasn't been flagged yet, compare it to all the others
        if (flags[h] == 0)
        {
            for (i = 2; i < realSize; i++)
            {
                if (flags[i] == 0)
                {
                    total = 0;
                    compare_rows(matrix[currentLead], matrix[i], realSize, &total);
                    //printf("Comparing %s to %s ---> Total: %f\n", labels[i], labels[currentLead], total);

                    if (total < ALPHA)
                    {
                        //printf("       Adding %s to the %s group.\n", labels[i], labels[currentLead]);
                        // Flag this row!
                        flags[i] = 1;
                        // Add the LEADER'S label to the epsilon table
                        strcpy(lookupTable[lastEl].epsilon, labels[currentLead]);
                        // Add the follower's vector to the epsilon table
                        strcpy(lookupTable[lastEl].vector, labels[i]);
                        lastEl++;
                    }
                }
            }
        }
        currentLead++;
    }

    int size = lastEl;
    int uniqueEls = 0;
    for (i = 0; i < size; i++)
    {
        for (j = i + 1; j < size; j++)
            if (strcmp(lookupTable[i].epsilon, lookupTable[j].epsilon) == 0)
                break;
        if (j == size)
            uniqueEls++;
    }

    return uniqueEls;
}

int compare_rows(int *row1, int *row2, int realSize, double *total)
{
    int sum1 = 0;
    int sum2 = 0;

    for (int i = 0; i < realSize; i++)
    {
        sum1 += row1[i];
        sum2 += row2[i];
    }
    // printf("sum1: %d\n", sum1);
    // printf("sum2: %d\n", sum2);

    double k1 = 0;
    if (sum1 != 0)
    {
        k1 = sqrt(sum2 / sum1);
    }
    double k2 = 0;
    if (sum2 != 0)
    {
        k2 = sqrt(sum1 / sum2);
    }
    // printf("k1: %f\n", k1);
    // printf("k2: %f\n", k2);

    for (int k = 1; k < realSize; k++)
    {
        // printf("row1[k]: %d\n", row1[k]);
        // printf("row2[k]: %d\n", row2[k]);
        double denominator = row1[k] + row2[k];
        if (denominator != 0)
        {
            // ********* pow(x, y) = x raised to the power of y
            double numerator = pow((k1 * row1[k] - k2 * row2[k]), 2.0);

            // printf("k1 * row1[k]: %f\n", k1 * row1[k]);
            // printf("k2 * row2[k]: %f\n", k2 * row2[k]);
            // printf("numerator: %f\n", numerator);
            // printf("denominator: %f\n", denominator);
            // printf("diff: %f\n", (numerator / denominator));
            *total += (numerator / denominator);
            // printf("Total: %f\n", *total);
            // printf("\n\n");
        }
    }

    return 1;
}