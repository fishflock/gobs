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

int debug = 0;

int print_c_matrix(int realSize, char labels[MAX_SIZE][5], int matrix[MAX_SIZE][MAX_SIZE]);
int print_labels(int realSize, char labels[MAX_SIZE][5]);
int print_e_lookup(struct luRow *lookupTable, int realSize, char labels[MAX_SIZE][5]);

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

    // Create list of labels and initialize matrix to all zeros
    char labels[MAX_SIZE][dir + 1];
    int matrix[MAX_SIZE][MAX_SIZE];
    for (int i = 0; i < MAX_SIZE; i++)
    {
        for (int j = 0; j < MAX_SIZE; j++)
            matrix[i][j] = 0;
    }

    // Create c matrix
    int realSize = create_c_matrix(hist, dir, labels, matrix, input_file);
    printf("real size: %d\n", realSize);

    if (debug)
    {
        int printCMat = print_c_matrix(realSize, labels, matrix);
        int printLabels = print_labels(realSize, labels);
    }

    // Create the epsilon lookup table
    struct luRow lookupTable[100];

    // Return # of group leaders ???
    int numGroups = create_e_table(lookupTable, dir, realSize, matrix, labels, ALPHA);

    if (debug)
    {
        int printEMat = print_e_lookup(lookupTable, realSize, labels);
    }

    int eMat[numGroups][numGroups];
    for (int i = 0; i < realSize; i++)
    {
        for (int j = 0; j < realSize; j++)
            eMat[i][j] = 0;
    }
    int sizeofLookupTable = sizeof(lookupTable) / sizeof(lookupTable[0]);
    int done = create_e_matrix(lookupTable, sizeofLookupTable, numGroups, eMat, input_file, output_file);

    fclose(input_file);
    fclose(output_file);
}

//##################################################################################
//##################################################################################
//##################################################################################
// HELPER METHODS FOR PRINTING TO TERMINAL

int print_c_matrix(int realSize, char labels[MAX_SIZE][5], int matrix[MAX_SIZE][MAX_SIZE])
{

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

int print_labels(int realSize, char labels[MAX_SIZE][5])
{
    printf("\n\nLabels:\n");
    for (int i = 1; i < realSize; i++)
    {
        printf("%d: %s\n", i, labels[i]);
    }
    return 1;
}

int print_e_lookup(struct luRow *lookupTable, int realSize, char labels[MAX_SIZE][5])
{
    printf("\n\nEpsilon Lookup Table: \n");
    for (int i = 0; i < realSize; i++)
    {
        if (isdigit(lookupTable[i].epsilon[1]))
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