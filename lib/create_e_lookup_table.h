#ifndef CREATE_E_LOOKUP_TABLE_H
#define CREATE_E_LOOKUP_TABLE_H
#include <stdio.h>
#include "structs.h"

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
int create_e_table(struct luRow **lookupTable, int dir, int realSize, double ***ptrToMatrix, char ***ptrToLabels, double ALPHA, int STAT_METHOD);

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
int fill_e_table(int *flags, struct luRow **lookupTable, int dir, int realSize, double ***ptrToMatrix, char ***ptrToLabels, double ALPHA, int STAT_METHOD);

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
int g_test(double *leadRow, double *otherRow, int realSize, double *total);

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
int chi_squared_test(double *row1, double *row2, int realSize, double *total);

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
double chsppf(double chiSquaredVal, int rowLength);

/**
 * This function converts the C-matrix values to probabilities. This prepares the matrix 
 * for statistical analysis.
 * 
 * @param realSize: Number of rows/columns in the C-matrix
 * @param ptrToMatrix: Pointer to the C-matrix
 */
int convert_to_probabilities(int realSize, double ***ptrToMatrix);

#endif