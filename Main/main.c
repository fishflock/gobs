#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../lib/read_c_vectors.h"
#include "../lib/create_c_matrix.h"
#include "../lib/structs.h"
#include "../lib/create_e_lookup_table.h"
#include "../lib/create_e_matrix.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

int BUFF_SIZE = 128;
int MAX_SIZE = 816;
char *DEF_INPUT_FILE = "sample";
char *DEF_OUTPUT_FILE = "compressvector2";
char *DEF_INPUT_FILEPATH = "input/";
char *DEF_OUTPUT_FILEPATH = "output/";
double ALPHA = 0.0;

int debug = 1;

int print_c_matrix(int realSize, char ***ptrToLabels, int ***ptrToMatrix);
int print_labels(int realSize, char ***ptrToLabels);
int print_e_lookup(struct luRow **ptrToLookupTable, int realSize, char ***ptrToLabels);

int main()
{
    FILE *input_file;
    FILE *output_file;
    char filename[BUFF_SIZE];
    char buffer[BUFF_SIZE];
    struct stat st = {0};

    // Open input file
    strcpy(filename, DEF_INPUT_FILE);
    printf("***Note: Input file must be CSV.\n");
    printf("       Enter name of input file WITHOUT the .csv extension = ");
    fgets(buffer, BUFF_SIZE, stdin);
    sscanf(buffer, "%s", filename);
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

    // Set past/future raw vector length to 5
    int hist = 5;
    // Set number of directions to 4 (compressed vec length=4)
    int dir = 4;

    int i;
    // INITIALIZE LABELS AS A 2D CHAR ARRAY WITH DIMENSIONS: MAX_SIZE*5
    char **labels = NULL;
    labels = (char **)malloc(sizeof(char *) * MAX_SIZE);
    for (i = 0; i < MAX_SIZE; i++)
    {
        labels[i] = (char *)malloc(dir + 1);
    }
    for (i = 0; i < dir; i++)
    {
        strcpy(labels[i], "0000");
    }

    // INITIALIZE MATRIX AS A 2D INT ARRAY WITH DIMENSIONS: MAX_SIZE*MAX_SIZE
    int **matrix = NULL;
    matrix = (int **)malloc(sizeof(int *) * MAX_SIZE);
    for (i = 0; i < MAX_SIZE; i++)
    {
        matrix[i] = (int *)malloc(MAX_SIZE);
    }
    for (int i = 0; i < MAX_SIZE; i++)
    {
        for (int j = 0; j < MAX_SIZE; j++)
            matrix[i][j] = 0;
    }

    // Create c matrix
    int realSize = create_c_matrix(dir, &labels, &matrix, input_file);
    printf("real size: %d\n", realSize);

    if (debug)
    {
        int printCMat = print_c_matrix(realSize, &labels, &matrix);
        int printLabels = print_labels(realSize, &labels);
    }

    // Create the epsilon lookup table
    //struct luRow lookupTable[100];
    struct luRow *lookupTable = (struct luRow *)malloc(sizeof(struct luRow) * MAX_SIZE);

    // Return # of group leaders ???
    int numGroups = create_e_table(&lookupTable, dir, realSize, &matrix, &labels, ALPHA);
    if (debug)
    {
        int printEMat = print_e_lookup(&lookupTable, realSize, &labels);
    }

    // INITIALIZE MATRIX AS A 2D INT ARRAY WITH DIMENSIONS: NUMGROUPS*NUMGROUPS
    int **eMat = NULL;
    eMat = (int **)malloc(sizeof(int *) * numGroups);
    for (i = 0; i < numGroups; i++)
    {
        eMat[i] = (int *)malloc(numGroups);
    }
    for (int i = 0; i < numGroups; i++)
    {
        for (int j = 0; j < numGroups; j++)
            eMat[i][j] = 0;
    }

    int sizeofLookupTable = sizeof(lookupTable) / sizeof(lookupTable[0]);
    int done = create_e_matrix(&lookupTable, 20, numGroups, &eMat, input_file, output_file);

    fclose(input_file);
    fclose(output_file);

    if (labels != NULL)
    {
        for (i = 0; i < MAX_SIZE; i++)
        {
            free(labels[i]);
            labels[i] = NULL;
        }
        free(labels);
        labels = NULL;
    }
    if (matrix != NULL)
    {
        for (i = 0; i < MAX_SIZE; i++)
        {
            free(matrix[i]);
            matrix[i] = NULL;
        }
        free(matrix);
        matrix = NULL;
    }
    if (eMat != NULL)
    {
        for (i = 0; i < numGroups; i++)
        {
            free(eMat[i]);
            eMat[i] = NULL;
        }
        free(eMat);
        eMat = NULL;
    }
}

//##################################################################################
//##################################################################################
//##################################################################################
// HELPER METHODS FOR PRINTING TO TERMINAL

int print_c_matrix(int realSize, char ***ptrToLabels, int ***ptrToMatrix)
{
    char **labels = *ptrToLabels;
    int **matrix = *ptrToMatrix;
    printf("\n\nCo-occurrence Matrix: \n");
    for (int i = 1; i < realSize; i++)
    {
        printf("%s ", labels[i]);
        for (int j = 1; j < realSize; j++)
            printf("%d ", matrix[i][j]);
        printf("\n");
    }
    return 1;
}

int print_labels(int realSize, char ***ptrToLabels)
{
    char **labels = *ptrToLabels;
    printf("\n\nLabels:\n");
    for (int i = 1; i < realSize; i++)
    {
        printf("%d: %s\n", i, labels[i]);
    }
    return 1;
}

int print_e_lookup(struct luRow **ptrToLookupTable, int realSize, char ***ptrToLabels)
{
    struct luRow *lookupTable = *ptrToLookupTable;
    char **labels = *ptrToLabels;
    printf("\n\nEpsilon Lookup Table: \n");
    for (int i = 0; i < realSize; i++)
    {
        if (isdigit(lookupTable[i].epsilon[1]))
        {
            printf("%s : ", lookupTable[i].epsilon);
            printf("%s\n", lookupTable[i].vector);
            // for (int k = 0; k < 4; k++)
            // {
            //     printf("%d", lookupTable[i].vector[k]);
            //     if (k == 3)
            //     {
            //         printf("\n");
            //     }
            // }
        }
    }
    printf("\n");
    return 1;
}