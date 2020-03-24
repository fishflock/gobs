#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/read_c_vectors.h"
#include "../lib/create_c_matrix.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

int BUFF_SIZE = 128;
char *DEF_INPUT_FILE = "sample";
char *DEF_OUTPUT_FILE = "compressvector2";
char *DEF_INPUT_FILEPATH = "input/";
char *DEF_OUTPUT_FILEPATH = "output/";
double ALPHA = 0.0;

struct luRow
{
    char epsilon[5];
    int vector[4];
};

int find_groups(int *row1, int *row2, int size, double *total);

int main()
{
    FILE *input_file;
    FILE *output_file;
    char filename[BUFF_SIZE];
    char buffer[BUFF_SIZE];
    struct stat st = {0};

    // Open input file
    strcpy(filename, DEF_INPUT_FILE);
    //printf("open file(%s)=", DEF_INPUT_FILE);
    //fgets(buffer, BUFF_SIZE, stdin);
    //sscanf(buffer, "%s", filename);
    strcat(filename, ".csv");
    strcpy(buffer, DEF_INPUT_FILEPATH);
    if (stat(buffer, &st) == -1)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    strcat(buffer, filename);
    if ((input_file = fopen(buffer, "r")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Open output file
    strcpy(filename, DEF_OUTPUT_FILE);
    //printf("open file(%s)=", DEF_OUTPUT_FILE);
    //fgets(buffer, BUFF_SIZE, stdin);
    //sscanf(buffer, "%s", filename);
    strcat(filename, ".txt");
    strcpy(buffer, DEF_OUTPUT_FILEPATH);
    if (stat(buffer, &st) == -1)
        mkdir(buffer, 0777);
    strcat(buffer, filename);
    if ((output_file = fopen(buffer, "w")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    /*int past[4];
        int future[4];
        while (read_cvec(past, future, input_file) != -1) {
            fprintf(output_file,"(");
            for (int i = 0; i < 4; i++) {
                fprintf(output_file, "%d", past[i]);
                if (i != 3) fprintf(output_file, " ");
            }
            fprintf(output_file, ") : (");
            for (int i = 0; i < 4; i++) {
                fprintf(output_file, "%d", future[i]);
                if (i != 3) fprintf(output_file, " ");
            }
            fprintf(output_file, ")\n");
        }*/

    int hist = 5;
    int dir = 4;
    int size = max_vectors(hist, dir);
    size = 20;
    char labels[size][dir + 1];
    int matrix[size][size];
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            matrix[i][j] = 0;
    }

    int x = create_c_matrix_default(size, labels, matrix, input_file);

    for (int i = 1; i < size; i++)
    {
        printf("%s ", labels[i]);
        for (int j = 1; j < size; j++)
            printf("%d ", matrix[i][j]);
        printf("\n");
    }

    printf("\nlabels:\n");
    for (int i = 1; i < size; i++)
    {
        printf("%d: %s\n", i, labels[i]);
    }

    //##################################################################################

    // Create flags array (each flag corresponds to a row)
    int flags[size];
    flags[0] = 1;
    flags[1] = 1;
    for (int i = 2; i < size; i++)
    {
        flags[i] = 0;
    }

    // Create the epsilon lookup table
    struct luRow testTable[3];
    // Add first row of C matrix to the epsilon lookup table
    for (int j = 0; j < dir; j++)
    {
        testTable[0].epsilon[j + 1] = labels[1][j + 1];
    }
    // Add the corresponding vector of first row to the epsilon table
    for (int j = 0; j < dir; j++)
    {
        testTable[0].vector[j] = labels[1][j] - '0';
    }

    int lastEl = 0;
    int currentLead = 1;
    double total = 0;
    // Loop through every row in matrix
    for (int i = 2; i < size; i++)
    {
        // IF it hasn't been flagged yet, compare it to all the others
        if (flags[i] == 0)
        {

            for (int i = 2; i < size; i++)
            {
                if (flags[i] == 0)
                {
                    total = 0;
                    find_groups(matrix[currentLead], matrix[i], size, &total);
                    //printf("Total: %f\n", total);

                    if (total > ALPHA)
                    {
                        printf("Adding label %s to the epsilon table!\n", labels[currentLead]);
                        // Flag this row!
                        flags[i] = 1;
                        // Add the LEADER'S label to the epsilon table
                        for (int j = 0; j < dir; j++)
                        {
                            testTable[0].epsilon[j + 1] = labels[currentLead][j + 1];
                        }
                        // Add the follower's vector to the epsilon table
                        for (int j = 0; j < dir; j++)
                        {
                            testTable[0].vector[j] = labels[i + 1][j];
                        }
                        lastEl++;
                    }
                }
            }
        }
        currentLead++;
    }

    fclose(input_file);
    fclose(output_file);
}

int find_groups(int *row1, int *row2, int size, double *total)
{
    //---------- DONT FORGET TO UN-HARDCODE SIZE ----------------------
    size = 7;

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
