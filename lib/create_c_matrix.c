#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_c_vectors.h"
#include "create_c_matrix.h"

// Note: expects 2D array for both labels and matrix.
// Must be uninitialized
int create_c_matrix(int dir,
                    char ***ptrToLabels,
                    int ***ptrToMatrix,
                    FILE *file)
{
    char **labels = *ptrToLabels;
    int **matrix = *ptrToMatrix;
    int past[dir];
    int future[dir];

    int last_label_pos = 0;
    int i;
    while (read_cvec(past, future, file) != -1)
    {
        char past_label[dir + 1];
        for (i = 0; i < dir; i++)
            past_label[i] = past[i] + '0';
        past_label[dir] = '\0';
        //printf("%s : ", past_label);

        char future_label[dir + 1];
        for (int j = 0; j < dir; j++)
            future_label[j] = future[j] + '0';
        future_label[dir] = '\0';
        //printf("%s\n", future_label);

        int past_pos = -1;
        for (int i = 0; i < last_label_pos + 1; i++)
        {
            if (strcmp(past_label, labels[i]) == 0)
            {
                //printf("Found a PAST match\n");
                past_pos = i;
            }
        }
        if (past_pos == -1)
        {
            //printf("Couldn't find a PAST match. Adding a new label!\n");
            last_label_pos++;
            strcpy(labels[last_label_pos], past_label);
            past_pos = last_label_pos;
        }

        int future_pos = -1;
        for (int i = 0; i < last_label_pos + 1; i++)
        {
            if (strcmp(future_label, labels[i]) == 0)
            {
                //printf("Found a FUTURE match\n");
                future_pos = i;
            }
        }
        if (future_pos == -1)
        {
            //printf("Couldn't find a FUTURE match. Adding a new label!\n");
            last_label_pos++;
            strcpy(labels[last_label_pos], future_label);
            future_pos = last_label_pos;
        }

        matrix[past_pos][future_pos]++;
    }
    fseek(file, 0, SEEK_SET);
    return last_label_pos + 1;
}
