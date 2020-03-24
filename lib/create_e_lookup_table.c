#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "create_e_lookup_table.h"
#include "structs.h"

int create_e_table(struct luRow *lookupTable, int dir, int size, int matrix[size][size], char labels[size][dir + 1], double ALPHA)
{
    // Create flags array (each flag corresponds to a row)
    int flags[size];
    flags[0] = 1;
    flags[1] = 1;
    for (int i = 2; i < size; i++)
    {
        flags[i] = 0;
    }

    // Add first row of C matrix to the epsilon lookup table
    strcpy(lookupTable[0].epsilon, labels[1]);

    // Add the corresponding vector of first row to the epsilon table
    for (int j = 0; j < dir; j++)
    {
        lookupTable[0].vector[j] = labels[1][j] - '0';
    }
    int fillTab = fill_e_table(flags, lookupTable, dir, size, matrix, labels, ALPHA);
    return 1;
}

int fill_e_table(int *flags, struct luRow *lookupTable, int dir, int size, int matrix[size][size], char labels[size][dir + 1], double ALPHA)
{
    int lastEl = 0;
    int currentLead = 1;
    double total = 0;
    // Loop through every row in matrix
    for (int h = 2; h < size; h++)
    {
        //printf("...labels[currentLead]: %s\n", labels[currentLead]);
        // IF it hasn't been flagged yet, compare it to all the others
        if (flags[h] == 0)
        {

            for (int i = 2; i < size; i++)
            {
                if (flags[i] == 0)
                {
                    total = 0;
                    compare_rows(matrix[currentLead], matrix[i], size, &total);
                    //printf("Total: %f\n", total);

                    if (total > ALPHA)
                    {
                        //printf("Adding %s to the %s group.\n", labels[i], labels[currentLead]);
                        // Flag this row!
                        flags[i] = 1;
                        // Add the LEADER'S label to the epsilon table
                        strcpy(lookupTable[lastEl].epsilon, labels[currentLead]);
                        // Add the follower's vector to the epsilon table
                        for (int j = 0; j < dir; j++)
                        {
                            lookupTable[lastEl].vector[j] = labels[i][j] - '0';
                        }
                        lastEl++;
                    }
                }
            }
        }
        currentLead++;
    }
    return lastEl + 1;
}

int compare_rows(int *row1, int *row2, int size, double *total)
{
    int sum1 = 0;
    int sum2 = 0;

    for (int i = 0; i < size; i++)
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

    for (int k = 0; k < size; k++)
    {
        double denominator = row1[k] + row2[k];
        if (denominator != 0)
        {
            // ********* pow(x, y) = x raised to the power of y
            double numerator = pow((k1 * row1[k] - k2 * row2[k]), 2.0);
            // printf("row1[k]: %d\n", row1[k]);
            // printf("row2[k]: %d\n", row2[k]);
            // printf("k1 * row1[k]: %f\n", k1 * row1[k]);
            // printf("k2 * row2[k]: %f\n", k2 * row2[k]);
            // printf("numerator: %f\n", numerator);
            // printf("denominator: %f\n", denominator);
            *total += numerator / denominator;
        }
    }

    return 1;
}