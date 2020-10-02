#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_c_vectors.h"
#include "create_c_matrix.h"

// Note: expects 2D array for both labels and matrix.
// Must be uninitialized
int create_c_matrix(int dir,
                    char ***ptrToLabels,
                    double ***ptrToMatrix,
                    FILE *file)
{
    char **labels = *ptrToLabels;
    double **matrix = *ptrToMatrix;
    char past[dir + 1];
    char future[dir + 1];

    int last_label_pos = 0;
    int i;
    int numVecsRecorded = 0;
    while (read_cvec(past, future, file) != -1)
    {
        int past_pos = -1;
        for (int i = 0; i < last_label_pos + 1; i++)
        {
            if (strcmp(past, labels[i]) == 0)
            {
                //printf("Found a PAST match\n");
                past_pos = i;
            }
        }
        if (past_pos == -1)
        {
            //printf("Couldn't find a PAST match. Adding a new label!\n");
            last_label_pos++;
            strcpy(labels[last_label_pos], past);
            past_pos = last_label_pos;
        }

        int future_pos = -1;
        for (int i = 0; i < last_label_pos + 1; i++)
        {
            if (strcmp(future, labels[i]) == 0)
            {
                //printf("Found a FUTURE match\n");
                future_pos = i;
            }
        }
        if (future_pos == -1)
        {
            //printf("Couldn't find a FUTURE match. Adding a new label!\n");
            last_label_pos++;
            strcpy(labels[last_label_pos], future);
            future_pos = last_label_pos;
        }

        matrix[past_pos][future_pos]++;
        numVecsRecorded++;
    }
    printf("\n\n NUM VEC RECORDED: %d\n", numVecsRecorded);
    fseek(file, 0, SEEK_SET);
    return last_label_pos + 1;
}
