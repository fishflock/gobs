/**
 * This file contains functions that can be used to create the Epsilon Lookup Table. 
 * This includes the functions that perform statistical comparison on rows and functionality 
 * related to the statistical analysis (i.e. converting the C-matrix to probabilities). 
 * The current stat tests implemented are chi-squared and g-test.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "create_e_lookup_table.h"

// Estimate of Euler's number
#define EULERS 2.71828

/**
 * This function uses the C-matrix and a number of other factors (see parameter details for 
 * more info on these factors) to create an Epsilon Lookup Table for the data. 
 * 
 * Note: The Epsilon Lookup Table is an array of "luRow" structs (where every "luRow" struct 
 *          represents a row in the table). This table holds info entailing which C-matrix 
 *          rows (behavioral primitives) are grouped together.
 * 
 * @param ptrToLookupTable: Pointer to the array of luRow structs to represent rows of the 
 *                          lookup table (memory should be allocated to the correct size with 
 *                          no real data in it yet)
 * @param dir: Number of directions used in data collection (user-specified)
 * @param realSize: Number of rows/columns in the C-matrix
 * @param ptrToMatrix: Pointer to the C-matrix
 * @param ptrToLabels: Pointer to the array of strings that are the labels for the C-matrix
 * @param ALPHA: The grouping threshold specified by the user (normally ranges between .001-.1)
 * @param STAT_METHOD: The statistical test specified by the user (1 if the user wants to 
 *                      apply the chi-squared test and 2 if the user wants to apply the g-test)
 * 
 * @return: (int) Number of rows/columns of E-matrix
 */
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

    // At this point, space for the table has been allocated and the table
    //  contains a single entry for the first compressed vector read in.
    //  Now use the call below to fill the rest of the table
    int fillTab = fill_e_table(flags, &lookupTable, dir, realSize, &matrix, &labels, ALPHA, STAT_METHOD);
    return fillTab;
}

/**
 * This function calls the necessary functions to perform statistical comparison on rows 
 * and fills in the Epsilon Lookup Table with the correct information based on a full 
 * statistical analysis of the C-matrix. This function loops through the C-matrix and 
 * compares each row to every other row that has not already been grouped.
 * 
 * @param flags: An array of ints used to keep track of which rows have already been grouped 
 *               (and therefore, do not need to be comapred again)
 * @param ptrToLookupTable: Pointer to the array of luRow structs to represent rows of the 
 *                          lookup table (memory should be allocated to the correct size with 
 *                          no real data in it yet)
 * @param dir: Number of directions used in data collection (user-specified)
 * @param realSize: Number of rows/columns in the C-matrix
 * @param ptrToMatrix: Pointer to the C-matrix
 * @param ALPHA: The grouping threshold specified by the user (normally ranges between .001-.1)
 * @param STAT_METHOD: The statistical test specified by the user (1 if the user wants to 
 *                      apply the chi-squared test and 2 if the user wants to apply the g-test)
 * 
 * @return: (int) Number of rows/columns of E-matrix
 */
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
        // IF it's the first row or the current row  hasn't been flagged yet,
        //  compare it to all the other rows
        if ((currentLead == 0) || (flags[currentLead] == 0))
        {
            // Compare the current lead against every other non-flagged row
            for (i = 1; i < realSize; i++)
            {
                if (flags[i] == 0)
                {
                    total = 0;

                    //Run the correct stat test based on user specified parameter
                    if (STAT_METHOD == 1)
                    {
                        chi_squared_test(matrix[currentLead], matrix[i], realSize, &total);
                    }
                    if (STAT_METHOD == 2)
                    {
                        g_test(matrix[currentLead], matrix[i], realSize, &total);
                    }

                    //If the current row is found to be similar to the current lead
                    // do the inside of this if statement
                    if (total < ALPHA)
                    {
                        //Keep track of the number of unique group leads for easier
                        // matrix indexing (when creating the e matrix)
                        if (strcmp(lookupTable[lastEl - 1].epsilon, labels[currentLead]) != 0)
                        {
                            epsilonIdx++;
                        }

                        // Flag this row!
                        flags[i] = 1;
                        // Add the LEADER'S label to the epsilon table
                        strcpy(lookupTable[lastEl].epsilon, labels[currentLead]);
                        // Add the follower's vector to the epsilon table
                        strcpy(lookupTable[lastEl].vector, labels[i]);
                        // Set the epsilon index to the index corresponding to the group lead
                        lookupTable[lastEl].epsilonIdx = epsilonIdx;
                        // Keep track of the size of the lookup table (all entries, not just
                        //  the number of unique group leads)
                        lastEl++;
                    }
                }
            }
        }
    }

    //After completing the long process above^, if there are still rows that have
    // not been flagged, add them as their own individual entry in the lookup table
    for (i = 0; i < realSize; i++)
    {
        if (flags[i] == 0)
        {
            // Update the epsilon index to keep track of the row's index in the e-matrix
            epsilonIdx++;
            // Flag this row
            flags[i] = 1;
            // Add the LEADER'S label to the epsilon table
            strcpy(lookupTable[lastEl].epsilon, labels[i]);
            // Add the follower's vector to the epsilon table
            strcpy(lookupTable[lastEl].vector, labels[i]);
            // Set the epsilon index to the index corresponding to the group lead
            lookupTable[lastEl].epsilonIdx = epsilonIdx;
            // Keep track of the size of the lookup table (all entries, not just
            //  the number of unique group leads)
            lastEl++;
        }
    }

    //Find the total number of unique group leads in the lookup table
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

/**
 * This function performs a g-test on the two rows passed as parameters.
 * 
 * @param leadRow: The "lead" row to compare the current row (row that has not been 
 *                  grouped yet) against
 * @param otherRow: The current row (row that has not been grouped yet)
 * @param realSize: Number of rows/columns in the C-matrix
 * @param total: The g-test value (if this value is less than ALPHA, then the "otherRow" 
 *                  can be considered statistically similar to the "leadRow")
 */
int g_test(double *leadRow, double *otherRow, int realSize, double *total)
{
    double currentTotal = 0.0;

    //Compute the g-test p-value between the current group lead and the current vector
    for (int k = 0; k < realSize; k++)
    {
        if (leadRow[k] > 0.0)
        {
            if ((otherRow[k] / leadRow[k]) > 0.0)
            {
                currentTotal += otherRow[k] * (log(otherRow[k] / leadRow[k]));
            }
        }
    }
    *total = 2 * currentTotal;

    return 1;
}

/**
 * This function performs a chi-squared test on the two rows passed as parameters.
 * 
 * @param row1: The "lead" row to compare the current row (row that has not been 
 *                  grouped yet) against
 * @param row2: The current row (row that has not been grouped yet)
 * @param realSize: Number of rows/columns in the C-matrix
 * @param total: The g-test value (if this value is less than ALPHA, then the "otherRow" 
 *                  can be considered statistically similar to the "leadRow")
 */
int chi_squared_test(double *row1, double *row2, int realSize, double *total)
{
    double sum1 = 0;
    double sum2 = 0;

    //Find the degree of freedom
    int degreeOfFreedom = 0;
    for (int i = 0; i < realSize; i++)
    {
        sum1 += row1[i];
        sum2 += row2[i];
        if ((row1[i] > 0) || (row2[i] > 0))
        {
            degreeOfFreedom++;
        }
    }

    //Compute preliminary values for the chi-squared calculation
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

    //Perform the summation calculations for the chi-squared value
    for (int k = 0; k < realSize; k++)
    {
        double denominator = row1[k] + row2[k];
        if (denominator != 0)
        {
            double numerator = pow((k1 * row1[k] - k2 * row2[k]), 2.0);

            *total += (numerator / denominator);
        }
    }

    //Adjust the chi-squared value to work with an alpha between 0 and 1
    // (this is done using the chi-squared percent point function)
    double adjustedChiSquared = 0.0;
    if (*total > 0.0)
    {
        adjustedChiSquared = chsppf(*total, degreeOfFreedom);
    }
    *total = adjustedChiSquared;

    return 1;
}

/**
 * This function applies the chi-squared percent point function to the chi-squared value 
 * calculated in the chi_squared_test function. This function was needed to account for 
 * the degrees of freedom (in this case in all non-zero bins in a row)
 * 
 * @param chiSquaredVal: The chi-squared value calculated in the chi_squared_test function
 * @param degreeOfFreedom: Degree of freedom value (all non-zero bins in a row))
 * 
 * @return: (double) Adjusted chi-squared value
 */
double chsppf(double chiSquaredVal, int degreeOfFreedom)
{
    //Compute preliminary values for the chi-squared percent point function calculation
    double eulersVal = pow(EULERS, (0 - chiSquaredVal) / 2);
    double xVal = pow(chiSquaredVal, (degreeOfFreedom / 2) - 1);
    double numerator = eulersVal * xVal;
    int x = (degreeOfFreedom / 2) - 1;

    //Calculate the gamma value
    double gammaVal = 1;
    for (int i = 1; i <= x; i++)
    {
        gammaVal = gammaVal * i;
    }

    //Final calculations
    double twosVal = pow(2.0, (degreeOfFreedom / 2));
    double denominator = twosVal * gammaVal;
    double alpha = numerator / denominator;
    return alpha;
}

/**
 * This function converts the C-matrix values to probabilities. This prepares the matrix 
 * for statistical analysis.
 * 
 * @param realSize: Number of rows/columns in the C-matrix
 * @param ptrToMatrix: Pointer to the C-matrix
 */
int convert_to_probabilities(int realSize, double ***ptrToMatrix)
{
    double **matrix = *ptrToMatrix;

    //Loop through the matrix and fine the sum of all values
    int i, j;
    double matrixSum = 0;
    for (i = 0; i < realSize; i++)
    {
        for (j = 0; j < realSize; j++)
        {
            matrixSum += matrix[i][j];
        }
    }
    printf("Matrix Sum = %f\n", matrixSum);

    //Divide each matrix value by the sum
    for (i = 0; i < realSize; i++)
    {
        for (j = 0; j < realSize; j++)
        {
            matrix[i][j] = matrix[i][j] / matrixSum;
        }
    }
    return 1;
}