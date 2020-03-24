#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_c_vectors.h"
#include "create_c_matrix.h"

// Note: expects 2D array for both labels and matrix.
// Must be uninitialized
int create_c_matrix(int len, int dir, int size,
                    char labels[size][dir + 1],
                    int matrix[size][size],
                    FILE *file)
{
    int past[dir];
    int future[dir];

    int last_label_pos = 0;
    for (int j = 0; j < dir; j++)
        labels[0][j] = '0';
    //labels[0][j] = past[j] + '0';
    labels[0][dir] = '\0';

    while (read_cvec(past, future, file) != -1)
    {
        char past_label[dir + 1];
        for (int j = 0; j < dir; j++)
            past_label[j] = past[j] + '0';
        past_label[dir] = '\0';

        char future_label[dir + 1];
        for (int j = 0; j < dir; j++)
            future_label[j] = future[j] + '0';
        future_label[dir] = '\0';

        int past_pos = -1;
        for (int i = 0; i < last_label_pos + 1; i++)
        {
            if (strcmp(past_label, labels[i]) == 0)
            {
                past_pos = i;
            }
        }
        if (past_pos == -1)
        {
            last_label_pos++;
            strcpy(labels[last_label_pos], past_label);
            past_pos = last_label_pos;
        }

        int future_pos = -1;
        for (int i = 0; i < last_label_pos + 1; i++)
        {
            if (strcmp(future_label, labels[i]) == 0)
            {
                future_pos = i;
            }
        }
        if (future_pos == -1)
        {
            last_label_pos++;
            strcpy(labels[last_label_pos], future_label);
            future_pos = last_label_pos;
        }

        matrix[past_pos][future_pos]++;
    }
    return 1;
}

int create_c_matrix_default(int size, char labels[size][4],
                            int matrix[size][size], FILE *file)
{
    create_c_matrix(5, 4, size, labels, matrix, file);
    return 1;
}

// TODO: overflow protection
unsigned long long int fact(int n)
{
    if (n < 1)
        return -1;
    if (n == 1)
        return 1;
    return n * fact(n - 1);
}

int n_choose_r(int n, int r)
{
    return (int)fact(n) / (fact(r) * fact(n - r));
}

// Based on number of integer solutions of a sum problem
// Dividing len steps into dir categories
// Use smaller calculation if possible
int max_vectors(int hist, int dir)
{
    int x;
    if (hist < dir - 1)
    {
        x = n_choose_r(hist + dir - 1, hist);
    }
    else
    {
        x = n_choose_r(hist + dir - 1, dir - 1);
    }
    return x;
}
