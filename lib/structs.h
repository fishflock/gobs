#ifndef LU_ROW_STRUCT
#define LU_ROW_STRUCT
#include <stdio.h>

/**
 * "luRow" is short for "lookup row"
 * 
 * This struct represents a row in the Epsilon Lookup Table.
 * 
 * epsilon: The group lead of the vector in question (the vector it was found 
 *          statistically similar to); In the form of a compressed vector 
 *          represented as a string
 * vector: The vector in question; In the form of a compressed vector 
 *          represented as a string
 * epsilonIdx: The index of the Epsilon matrix that the epsilon (and therefore, 
 *          the vector corresponds to)
 * 
 */
struct luRow
{
    char *epsilon;
    char *vector;
    int epsilonIdx;
};

#endif
