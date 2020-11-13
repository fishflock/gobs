#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "create_e_lookup_table.h"

#define EULERS 2.71828

int create_e_table(struct luRow **ptrToLookupTable, int dir, int realSize, double ***ptrToMatrix, char ***ptrToLabels, double ALPHA, int STAT_METHOD)
{
    struct luRow *lookupTable = *ptrToLookupTable;
    char **labels = *ptrToLabels;
    double **matrix = *ptrToMatrix;

    // Create flags array (each flag corresponds to a row)
    int flags[realSize];
    flags[0] = 1;
    for (int i = 1; i < realSize; i++)
    {
        flags[i] = 0;
    }

    // Add first row of C matrix to the epsilon lookup table
    strcpy(lookupTable[0].epsilon, labels[0]);

    // Add the corresponding vector of first row to the epsilon table
    strcpy(lookupTable[0].vector, labels[0]);

    int fillTab = fill_e_table(flags, &lookupTable, dir, realSize, &matrix, &labels, ALPHA, STAT_METHOD);
    return fillTab;
}

int fill_e_table(int *flags, struct luRow **ptrToLookupTable, int dir, int realSize, double ***ptrToMatrix, char ***ptrToLabels, double ALPHA, int STAT_METHOD)
{
    struct luRow *lookupTable = *ptrToLookupTable;
    char **labels = *ptrToLabels;
    double **matrix = *ptrToMatrix;

    int lastEl = 1;
    int currentLead = 0;
    double total = 0;
    int epsilonIdx = 0;
    int i, j;
    // Loop through every row in matrix
    for (currentLead = 0; currentLead < realSize; currentLead++)
    {
        //printf("...labels[currentLead]: %s\n", labels[currentLead]);
        // IF it hasn't been flagged yet, compare it to all the others
        if ((currentLead == 0) || (flags[currentLead] == 0))
        {
            for (i = 1; i < realSize; i++)
            {
                if (flags[i] == 0)
                {
                    total = 0;
                    if (STAT_METHOD == 1)
                    {
                        //printf("Performing X squared test\n");
                        compare_rows(matrix[currentLead], matrix[i], realSize, &total);
                    }
                    if (STAT_METHOD == 2)
                    {
                        //printf("Performing g test\n");
                        g_test(matrix[currentLead], matrix[i], realSize, &total);
                        //printf("Comparing %s to %s ---> Total: %f\n", labels[i], labels[currentLead], total);
                    }

                    if (total < ALPHA)
                    {
                        if (strcmp(lookupTable[lastEl - 1].epsilon, labels[currentLead]) != 0)
                        {
                            epsilonIdx++;
                        }

                        //printf("       Adding %s to the %s group.\n", labels[i], labels[currentLead]);
                        // Flag this row!
                        flags[i] = 1;
                        // Add the LEADER'S label to the epsilon table
                        strcpy(lookupTable[lastEl].epsilon, labels[currentLead]);
                        // Add the follower's vector to the epsilon table
                        strcpy(lookupTable[lastEl].vector, labels[i]);
                        lookupTable[lastEl].epsilonIdx = epsilonIdx;
                        lastEl++;
                    }
                }
            }
        }
    }

    int uniqueEls = 0;
    for (i = 0; i < lastEl; i++)
    {
        for (j = i + 1; j < lastEl; j++)
            if (strcmp(lookupTable[i].epsilon, lookupTable[j].epsilon) == 0)
                break;
        if (j == lastEl)
            uniqueEls++;
    }

    return uniqueEls;
}

int g_test(double *leadRow, double *otherRow, int realSize, double *total)
{
    double currentTotal = 0.0;
    for (int k = 0; k < realSize; k++)
    {
        if (leadRow[k] > 0.0)
        {
            if ((otherRow[k] / leadRow[k]) > 0.0)
            {
                currentTotal += otherRow[k] * (log(otherRow[k] / leadRow[k]));
            }
        }
        //printf("%f : ", currentTotal);
    }

    *total = 2 * currentTotal;
    //printf("\nG-Test val: %f\n", *total);
    return 1;
}

int compare_rows(double *row1, double *row2, int realSize, double *total)
{
    double sum1 = 0;
    double sum2 = 0;

    int degreeOfFreedom = 0;
    for (int i = 0; i < realSize; i++)
    {
        //printf("%f : %f\n", row1[i], row2[i]);
        sum1 += row1[i];
        sum2 += row2[i];
        if ((row1[i] > 0) || (row2[i] > 0))
        {
            degreeOfFreedom++;
        }
    }
    //printf("DEGREE OF FREEDOM: %d\n", degreeOfFreedom);
    // printf("sum1: %f\n", sum1);
    // printf("sum2: %f\n", sum2);

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

    for (int k = 0; k < realSize; k++)
    {
        // printf("row1[k]: %f\n", row1[k]);
        // printf("row2[k]: %f\n", row2[k]);
        double denominator = row1[k] + row2[k];
        if (denominator != 0)
        {
            // ********* pow(x, y) = x raised to the power of y
            double numerator = pow((k1 * row1[k] - k2 * row2[k]), 2.0);

            // printf("k1 * row1[k]: %f\n", k1 * row1[k]);
            // printf("k2 * row2[k]: %f\n", k2 * row2[k]);
            // printf("numerator: %f\n", numerator);
            // printf("denominator: %f\n", denominator);

            //*total += chsppf((numerator / denominator), realSize);
            *total += (numerator / denominator);

            // printf("Total: %f\n", *total);
            // printf("\n\n");
        }
    }

    //printf("Total: %f\n", *total);

    double adjustedChiSquared = 0.0;
    if (*total > 0.0)
    {
        adjustedChiSquared = chsppf(*total, degreeOfFreedom);
    }

    //printf("adjusted chi-squared : %f\n", adjustedChiSquared);
    *total = adjustedChiSquared;

    return 1;
}

double chsppf(double chiSquaredVal, int rowLength)
{
    double eulersVal = pow(EULERS, (0 - chiSquaredVal) / 2);
    double xVal = pow(chiSquaredVal, (rowLength / 2) - 1);
    double numerator = eulersVal * xVal;

    int x = (rowLength / 2) - 1;
    double gammaVal = 1;
    for (int i = 1; i <= x; i++)
    {
        gammaVal = gammaVal * i;
    }

    double twosVal = pow(2.0, (rowLength / 2));
    double denominator = twosVal * gammaVal;

    double alpha = numerator / denominator;
    //printf("numerator: %.0000f // denominator: %.0000f\n", numerator, denominator);
    return alpha;
}

int convert_to_probabilities(int realSize, double ***ptrToMatrix)
{
    double **matrix = *ptrToMatrix;
    int i, j;
    double matrixSum = 0;
    for (i = 0; i < realSize; i++)
    {
        for (j = 0; j < realSize; j++)
        {
            matrixSum += matrix[i][j];
        }
    }

    //matrixSum = 146;
    printf("Matrix Sum = %f\n", matrixSum);

    for (i = 0; i < realSize; i++)
    {
        for (j = 0; j < realSize; j++)
        {
            matrix[i][j] = matrix[i][j] / matrixSum;
            //printf("[%d, %d] = %f\n", i, j, matrix[i][j]);
        }
    }
    return 1;
}