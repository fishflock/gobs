#include <stdio.h>
#include <string.h>
#include "read_c_vectors.h"

#define DEF_HIST_LEN 5
#define DEF_DIR_NO 4

/*
 * read_c_vectors
 * 
 * Params:
 *  int * past: compression vector of past history vector, size dir
 *  int * future: compression vector of future history vector, size dir
 *  int len: size of history vector (past and future)
 *  int dir: number of possible directions
 *  FILE file: file to be read from
 *
 * Notes:
 *  Assumes directions start at 1, unlike array positions (start at 0).
 *  Assumes the arrays that the pointers point to are initialized to
 *  the size of the number of directions (dir).
 *
 * Returns:
 *  -1 on failure
 *  1 on success
 *
 */

int read_c_vectors(int * past, int * future, 
                    int len, int dir, FILE * file) {
    fpos_t pos;
    if (fgetpos(file, &pos) == -1) return -1;

    // Initialize vectors to all 0s
    for (int i = 0; i < dir; i++) {
        past[i] = 0;
        future[i] = 0;
    }

    int val;
    // Get past compression vector
    for (int i = 0; i < len; i++) {
        if (fscanf(file, " %d", &val) == EOF) return -1;
        if (val > dir) return -1;
        past[val - 1] += 1;
        if (fscanf(file, " ,") == EOF) return -1;
    }

    // Get future compression vector
    for (int i = 0; i < len; i++) {
        if (fscanf(file, " %d", &val) == EOF) return -1;
        if (val > dir) return -1;
        future[val - 1] += 1;
        if (fscanf(file, " ,") == EOF) {
            // e.g. end of file doesn't require comma
            if (i != len - 1) return -1;
        }
    }
    
    // Move forward
    if (fsetpos(file, &pos) == -1) return -1;
    if (fscanf(file, " %*d ,") == -1) return -1;
    
    return 1;
}

// Default values for read_c_vectors
int read_cvec(int * past, int * future, FILE * file) {
    int len = DEF_HIST_LEN;
    int dir = DEF_DIR_NO;
    return read_c_vectors(past, future, len, dir, file);
}
