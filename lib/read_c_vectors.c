/**
 * This file contains functions related to reading input data as vectors.
 */

#include <stdio.h>
#include <string.h>
#include "read_c_vectors.h"

/**
 * This function reads a past and future vector from the input file and COMPRESSES them. 
 * This function should be called repeatedly to loop through entire input file.
 * 
 * Notes:
 *  Assumes directions start at 1, unlike array positions (start at 0).
 *  Assumes the arrays that the pointers point to are initialized to
 *  the size of the number of directions (dir).
 * 
 * @param past: Compression vector of past history vector, size dir
 * @param future: Compression vector of future history vector, size dir
 * @param len: Size of history vector (past and future)
 * @param dir: Number of possible directions
 * @param file: File to be read from
 *
 * @return: -1 on failure; 1 on success
 */
int read_c_vectors(char *past, char *future,
                   int len, int dir, FILE *file)
{
    fpos_t pos;
    if (fgetpos(file, &pos) == -1)
        return -1;

    int i;
    int pastAsInts[dir];
    int futureAsInts[dir];
    // Initialize vectors to all 0s
    for (i = 0; i < dir; i++)
    {
        pastAsInts[i] = 0;
        futureAsInts[i] = 0;
    }

    int val;
    // Get past compression vector
    for (i = 0; i < len; i++)
    {
        if (fscanf(file, " %d", &val) == EOF)
            return -1;
        if (val > dir)
            return -1;
        pastAsInts[val - 1] += 1;
        if (fscanf(file, " ,") == EOF)
            return -1;
    }

    // Get future compression vector
    for (i = 0; i < len; i++)
    {
        if (fscanf(file, " %d", &val) == EOF)
            return -1;
        if (val > dir)
            return -1;
        futureAsInts[val - 1] += 1;
        if (fscanf(file, " ,") == EOF)
        {
            // e.g. end of file doesn't require comma
            if (i != len - 1)
                return -1;
        }
    }

    //Save past vector as a string
    for (i = 0; i < dir; i++)
        past[i] = pastAsInts[i] + '0';
    past[dir] = '\0';

    //Save future vector as a string
    for (int j = 0; j < dir; j++)
        future[j] = futureAsInts[j] + '0';
    future[dir] = '\0';

    // Move forward
    if (fsetpos(file, &pos) == -1)
        return -1;
    if (fscanf(file, " %*d ,") == -1)
        return -1;

    return 1;
}

/**
 * This function reads a vector from the input file. This function should be 
 * called repeatedly to loop through entire input file.
 * 
 * Notes:
 *  Assumes directions start at 1, unlike array positions (start at 0).
 *  Assumes the arrays that the pointers point to are initialized to
 *  the size of the number of directions (dir).
 * 
 * @param vector: Vector of past history vector, size len
 * @param len: Size of history vector (past and future)
 * @param file: File to be read from
 *
 * @return: -1 on failure; 1 on success
 */
int read_uncompressed_vectors(char *vector, int len, FILE *file)
{
    fpos_t pos;
    if (fgetpos(file, &pos) == -1)
        return -1;

    int i;
    int vec[len];
    // Initialize vectors to all 0s
    for (i = 0; i < len; i++)
    {
        vec[i] = 0;
    }

    // Read in a vector of the correct length (user specified param)
    int val;
    for (i = 0; i < len; i++)
    {
        if (fscanf(file, " %d", &vec[i]) == EOF)
            return -1;
        if (fscanf(file, " ,") == EOF)
            return -1;
    }

    //Save vector as a string
    for (i = 0; i < len; i++)
        vector[i] = vec[i] + '0';
    vector[len] = '\0';

    // Move forward
    if (fsetpos(file, &pos) == -1)
        return -1;
    if (fscanf(file, " %*d ,") == -1)
        return -1;

    return 1;
}
