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
//char *DEF_INPUT_FILE = "sample";
//char *DEF_OUTPUT_FILE = "compressvector2";
char *DEF_INPUT_FILEPATH = "Main/input/";
char *DEF_OUTPUT_FILEPATH = "output/";
double ALPHA = 0.0;

int debug = 1;

int print_c_matrix(int realSize, char ***ptrToLabels, double ***ptrToMatrix);
int print_labels(int realSize, char ***ptrToLabels);
int print_e_lookup(struct luRow **ptrToLookupTable, int realSize, char ***ptrToLabels);

int main(int argc, char *argv[])
{
    //--------------- Varify input and output files ---------------
    FILE *input, *output;
    char *input_file = (char *)malloc(sizeof(char)*BUFF_SIZE);
    char *output_file = (char *)malloc(sizeof(char)*BUFF_SIZE);
    input_file = argv[1];
    output_file = argv[2];

    //Check it input is .csv
    if (strcmp(&input_file[strlen(input_file) - 4], ".csv") == 0)
    {
        printf("Importing data from %s...\n", input_file);
    }
    else
    {
        printf("%s is an invalid file. Input must be a .csv file. Exiting program...\n", input_file);
        exit(1);
    }
    //Check if output is .txt
    if (strcmp(&output_file[strlen(output_file) - 4], ".txt") != 0)
    {
        printf("%s is an invalid file. output must be a .txt file. Exiting program...\n", output_file);
        exit(1);
    }
    else
    {
        output = fopen(output_file, "w");
    }
    //Check that input does exist
    if ((input = fopen(input_file, "r")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    //--------- Check normalize and reduceNoiseFlag parameters ---------
    int normalize;
    if (argv[3] == NULL)
    {
        normalize = 0;
    }
    else
    {
        normalize = atoi(argv[3]);
    }

    int reduceNoiseFlag;
    if (argv[4] == NULL)
    {
        reduceNoiseFlag = 0;
    }
    else
    {
        reduceNoiseFlag = atoi(argv[4]);
    }

    ALPHA = atof(argv[5]);

    //-------------------------------------------------------------------

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
        labels[i] = (char *)malloc(sizeof(char)*dir + 1);
    }
    for (i = 0; i < dir; i++)
    {
        strcpy(labels[i], "0000");
    }

    // INITIALIZE MATRIX AS A 2D INT ARRAY WITH DIMENSIONS: MAX_SIZE*MAX_SIZE
    double **matrix = NULL;
    matrix = (double **)malloc(sizeof(double *) * MAX_SIZE);
    for (i = 0; i < MAX_SIZE; i++)
    {
        matrix[i] = (double *)malloc(sizeof(double)*MAX_SIZE);
    }
    for (int i = 0; i < MAX_SIZE; i++)
    {
        for (int j = 0; j < MAX_SIZE; j++)
            matrix[i][j] = 0;
    }

    // Create c matrix
    int numVecsRecorded;
    int sizeOfCMatrix = create_c_matrix(dir, &labels, &matrix, input, &numVecsRecorded);
    printf("C matrix is %dx%d\n", sizeOfCMatrix, sizeOfCMatrix);
    printf("Number of unqiue vectors (size of labels array) %d\n", sizeOfCMatrix);
    if (debug)
    {
        int printCMat = print_c_matrix(sizeOfCMatrix, &labels, &matrix);
        int printLabels = print_labels(sizeOfCMatrix, &labels);
    }
    if (normalize)
    {
        int normalize = normalize_c_matrix(sizeOfCMatrix, &matrix, numVecsRecorded, 5);
        printf("C Matrix Normalized by Number of Vectors: \n");
        int normalizedMat = print_c_matrix(sizeOfCMatrix, &labels, &matrix);
    }

    // Create the epsilon lookup table
    //struct luRow lookupTable[100];
    struct luRow *lookupTable = (struct luRow *)malloc(sizeof(struct luRow) * MAX_SIZE);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // for (int i = 0; i < MAX_SIZE; i++)
    // {
    //     for (int j = 0; j < MAX_SIZE; j++)
    //         matrix[i][j] = 0;
    // }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // double **cenekTest = NULL;
    // cenekTest = (double **)malloc(sizeof(double *) * 2);
    // for (i = 0; i < 8; i++)
    // {
    //     cenekTest[i] = (double *)malloc(sizeof(double) * 8);
    // }
    // cenekTest[0][0] = 0;
    // cenekTest[0][1] = 1;
    // cenekTest[0][2] = 3;
    // cenekTest[0][3] = 1;
    // cenekTest[0][4] = 4;
    // cenekTest[0][5] = 1;
    // cenekTest[0][6] = 1;
    // cenekTest[0][7] = 3;

    // cenekTest[1][0] = 0;
    // cenekTest[1][1] = 1;
    // cenekTest[1][2] = 2;
    // cenekTest[1][3] = 1;
    // cenekTest[1][4] = 4;
    // cenekTest[1][5] = 1;
    // cenekTest[1][6] = 1;
    // cenekTest[1][7] = 2;

    // //sizeOfCMatrix = 8;
    // reduce_noise(sizeOfCMatrix, &cenekTest);

    // double total = 0;
    // compare_rows(cenekTest[0], cenekTest[1], sizeOfCMatrix, &total);
    if (reduceNoiseFlag)
    {
        reduce_noise(sizeOfCMatrix, &matrix);
        printf("C Matrix with Reduced Noise: \n");
        int reducedMat = print_c_matrix(sizeOfCMatrix, &labels, &matrix);
    }

    // Return # of group leaders ???
    int sizeOfEMatrix = create_e_table(&lookupTable, dir, sizeOfCMatrix, &matrix, &labels, ALPHA);
    printf("E matrix is %dx%d\n", sizeOfEMatrix, sizeOfEMatrix);
    if (debug)
    {
        int printEMat = print_e_lookup(&lookupTable, sizeOfCMatrix, &labels);
    }

    // INITIALIZE MATRIX AS A 2D INT ARRAY WITH DIMENSIONS: sizeOfEMatrix*sizeOfEMatrix
    int **eMat = NULL;
    eMat = (int **)malloc(sizeof(int *) * sizeOfEMatrix);

    for (i = 0; i < sizeOfEMatrix; i++)
    {
        eMat[i] = (int *)malloc(sizeof(int)*sizeOfEMatrix);
    }
    for (int i = 0; i < sizeOfEMatrix; i++)
    {
        for (int j = 0; j < sizeOfEMatrix; j++)
            eMat[i][j] = 0;
    }

    int done = create_e_matrix(&lookupTable, sizeOfCMatrix, sizeOfEMatrix, &eMat, input, output);

    fclose(input);
    fclose(output);

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
        for (i = 0; i < sizeOfEMatrix; i++)
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

int print_c_matrix(int sizeOfCMatrix, char ***ptrToLabels, double ***ptrToMatrix)
{
    char **labels = *ptrToLabels;
    double **matrix = *ptrToMatrix;
    printf("Co-occurrence Matrix: \n");
    for (int i = 0; i < sizeOfCMatrix; i++)
    {
        printf("%s ", labels[i]);
        for (int j = 0; j < sizeOfCMatrix; j++)
            printf("%.0f ", matrix[i][j]);
        printf("\n");
    }
    return 1;
}

int print_labels(int sizeOfCMatrix, char ***ptrToLabels)
{
    char **labels = *ptrToLabels;
    printf("\n\nLabels:\n");
    for (int i = 0; i < sizeOfCMatrix; i++)
    {
        printf("%d: %s\n", i, labels[i]);
    }
    return 1;
}

int print_e_lookup(struct luRow **ptrToLookupTable, int sizeOfCMatrix, char ***ptrToLabels)
{
    struct luRow *lookupTable = *ptrToLookupTable;
    char **labels = *ptrToLabels;
    printf("\n\nEpsilon Lookup Table: \n");
    for (int i = 0; i < sizeOfCMatrix; i++)
    {
        if (isdigit(lookupTable[i].epsilon[1]))
        {
            printf("%s : ", lookupTable[i].epsilon);
            printf("%s\n", lookupTable[i].vector);
        }
    }
    printf("\n");
    return 1;
}