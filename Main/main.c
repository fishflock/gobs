#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/read_c_vectors.h"
#include "../lib/create_c_matrix.h"
#include "../lib/create_e_lookup_table.h"
#include "../lib/create_e_matrix.h"
#include "../lib/structs.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

int BUFF_SIZE = 128;
char *DEF_INPUT_FILE = "sample";
char *DEF_OUTPUT_FILE = "compressvector2";
char *DEF_INPUT_FILEPATH = "input/";
char *DEF_OUTPUT_FILEPATH = "output/";
double ALPHA = 0.0;

int debug = 1;

int print_c_matrix(int size, char labels[size][5], int matrix[size][size]);
int print_labels(int size, char labels[size][5]);
int print_e_matrix(struct luRow *lookupTable, int size, char labels[size][5]);

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

    // // Open output file
    // strcpy(filename, DEF_OUTPUT_FILE);
    // //printf("open file(%s)=", DEF_OUTPUT_FILE);
    // //fgets(buffer, BUFF_SIZE, stdin);
    // //sscanf(buffer, "%s", filename);
    // strcat(filename, ".txt");
    // strcpy(buffer, DEF_OUTPUT_FILEPATH);
    // if (stat(buffer, &st) == -1)
    //     mkdir(buffer, 0777);
    // strcat(buffer, filename);
    // if ((output_file = fopen(buffer, "w")) == NULL)
    // {
    //     perror("fopen");
    //     exit(EXIT_FAILURE);
    // }

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

    // Set past/future raw vector length to 5
    int hist = 5;
    // Set number of directions to 4 (compressed vec length=4)
    int dir = 4;

    // TODO: Modify this to produce two sizes:
    //      1 for max vecs (including duplicates)
    //      1 for actual # vecs
    int size = max_vectors(hist, dir);
    size = 20;

    // Create list of labels and initialize matrix to all zeros
    char labels[size][dir + 1];
    int matrix[size][size];
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            matrix[i][j] = 0;
    }

    // Create c matrix
    int createCMat = create_c_matrix_default(size, labels, matrix, input_file);

    if (debug)
    {
        int printCMat = print_c_matrix(size, labels, matrix);
        int printLabels = print_labels(size, labels);
    }

    // Create the epsilon lookup table
    struct luRow lookupTable[10];

    // Return # of group leaders ???
    int numGroups = create_e_table(lookupTable, dir, size, matrix, labels, ALPHA);

    if (debug)
    {
        int printEMat = print_e_matrix(lookupTable, size, labels);
    }

    int eMat[numGroups][numGroups];
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            eMat[i][j] = 0;
    }
    int sizeofLookupTable = sizeof(lookupTable) / sizeof(lookupTable[0]);
    int done = create_e_matrix(lookupTable, sizeofLookupTable, numGroups, eMat, output_file);

    fclose(input_file);
    fclose(output_file);
}

//##################################################################################
//##################################################################################
//##################################################################################
// HELPER METHODS FOR PRINTING TO TERMINAL

int print_c_matrix(int size, char labels[size][5], int matrix[size][size])
{
    printf("\n\nCo-occurrence Matrix: \n");
    for (int i = 1; i < size; i++)
    {
        printf("%s ", labels[i]);
        for (int j = 1; j < size; j++)
            printf("%d ", matrix[i][j]);
        printf("\n");
    }
    return 1;
}

int print_labels(int size, char labels[size][5])
{
    printf("\n\nLabels:\n");
    for (int i = 1; i < size; i++)
    {
        printf("%d: %s\n", i, labels[i]);
    }
    return 1;
}

int print_e_matrix(struct luRow *lookupTable, int size, char labels[size][5])
{
    printf("\n\nEpsilon Lookup Table: \n");
    for (int i = 0; i < size; i++)
    {
        // if (strcmp((lookupTable[i].epsilon-'0'), "") != 0)
        if (isdigit(lookupTable[i].epsilon[0]))
        {
            printf("%s : ", lookupTable[i].epsilon);
            for (int k = 0; k < 4; k++)
            {
                printf("%d", lookupTable[i].vector[k]);
                if (k == 3)
                {
                    printf("\n");
                }
            }
        }
    }
    printf("\n");
    return 1;
}
