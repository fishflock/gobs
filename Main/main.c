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

int NORMALIZE_FLAG = 0;
double ALPHA = 0.0;
int VEC_HIST_LEN = 5;
int NUM_DIRECTIONS = 4;

int debug = 1;

int print_c_matrix(int realSize, char ***ptrToLabels, double ***ptrToMatrix);
int print_c_matrix_to_file(int sizeOfCMatrix, char ***ptrToLabels, double ***ptrToMatrix, FILE *output_file);
int print_labels(int realSize, char ***ptrToLabels);
int print_e_lookup(struct luRow **ptrToLookupTable, int realSize, char ***ptrToLabels);

int main(int argc, char *argv[])
{
    //--------------- Varify input and output files ---------------
    FILE *input, *output, *cMatOutput;
    char *input_file = (char *)malloc(BUFF_SIZE);
    char *output_file = (char *)malloc(BUFF_SIZE);
    char *output_file_c_mat = (char *)malloc(BUFF_SIZE);
    input_file = argv[1];
    output_file = argv[2];

    int extensionIdx = strlen(argv[2]) - 4;
    for (int i = 0; i < strlen(argv[2]); i++)
    {
        if (i == extensionIdx)
        {
            break;
        }
        output_file_c_mat[i] = argv[2][i];
    }
    strcat(output_file_c_mat, "_c_mat.txt");

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
        cMatOutput = fopen(output_file_c_mat, "w");
    }
    //Check that input does exist
    if ((input = fopen(input_file, "r")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    //--------- Check normalize and reduceNoiseFlag parameters ---------
    if (argv[3] != NULL)
    {
        NORMALIZE_FLAG = atoi(argv[3]);
        printf("Setting NORMALIZE_FLAG to: %d\n", NORMALIZE_FLAG);
    }
    if (argv[4] != NULL)
    {
        ALPHA = atof(argv[4]);
        printf("Setting ALPHA to: %f\n", ALPHA);
    }
    if (argv[5] != NULL)
    {
        VEC_HIST_LEN = atoi(argv[5]);
        printf("Setting VEC_HIST_LEN to: %d\n", VEC_HIST_LEN);
    }
    if (argv[6] != NULL)
    {
        NUM_DIRECTIONS = atoi(argv[6]);
        printf("Setting NUM_DIRECTIONS to: %d\n", NUM_DIRECTIONS);
    }

    //-------------------------------------------------------------------

    int i;
    // Create a string initialized to X zeros; where X is the number of directions specified
    // (Set the last character to the string terminator character)
    char initializeToZeros[NUM_DIRECTIONS + 1];
    for (i = 0; i < NUM_DIRECTIONS; i++)
    {
        initializeToZeros[i] = '0';
    }
    initializeToZeros[NUM_DIRECTIONS] = '\0';
    // INITIALIZE LABELS AS A 2D CHAR ARRAY WITH DIMENSIONS: MAX_SIZE*(NUM_DIRECTIONS + 1)
    char **labels = NULL;
    labels = (char **)malloc(sizeof(char *) * MAX_SIZE);
    for (i = 0; i < MAX_SIZE; i++)
    {
        labels[i] = (char *)malloc(sizeof(char) * NUM_DIRECTIONS + 1);
        strcpy(labels[i], initializeToZeros);
    }

    // INITIALIZE MATRIX AS A 2D INT ARRAY WITH DIMENSIONS: MAX_SIZE*MAX_SIZE
    double **matrix = NULL;
    matrix = (double **)malloc(sizeof(double *) * MAX_SIZE);
    for (i = 0; i < MAX_SIZE; i++)
    {
        matrix[i] = (double *)malloc(sizeof(double) * MAX_SIZE);
    }
    for (int i = 0; i < MAX_SIZE; i++)
    {
        for (int j = 0; j < MAX_SIZE; j++)
            matrix[i][j] = 0;
    }

    // Create c matrix
    int numVecsRecorded;
    int sizeOfCMatrix = create_c_matrix(NUM_DIRECTIONS, VEC_HIST_LEN, &labels, &matrix, input, &numVecsRecorded);
    printf("C matrix is %dx%d\n", sizeOfCMatrix, sizeOfCMatrix);
    printf("Number of unqiue vectors (size of labels array) %d\n", sizeOfCMatrix);
    if (debug)
    {
        int printCMat = print_c_matrix(sizeOfCMatrix, &labels, &matrix);
        int printLabels = print_labels(sizeOfCMatrix, &labels);
    }
    if (NORMALIZE_FLAG)
    {
        int normalize = normalize_c_matrix(sizeOfCMatrix, &matrix, numVecsRecorded, VEC_HIST_LEN);
        printf("C Matrix Normalized by Number of Vectors: \n");
        int normalizedMat = print_c_matrix(sizeOfCMatrix, &labels, &matrix);
    }

    int printCMat = print_c_matrix_to_file(sizeOfCMatrix, &labels, &matrix, cMatOutput);

    int probs = reduce_noise(sizeOfCMatrix, &matrix);
    printf("C Matrix converted to probabilities: \n");
    int probsMat = print_c_matrix(sizeOfCMatrix, &labels, &matrix);

    // Create the epsilon lookup table
    //struct luRow lookupTable[100];
    struct luRow *lookupTable = (struct luRow *)malloc(sizeof(struct luRow) * MAX_SIZE);

    for (i = 0; i < MAX_SIZE; i++)
    {
        lookupTable[i].epsilon = (char *)malloc(sizeof(char) * (NUM_DIRECTIONS + 1));
        lookupTable[i].vector = (char *)malloc(sizeof(char) * (NUM_DIRECTIONS + 1));
    }

    // Return # of group leaders ???
    int sizeOfEMatrix = create_e_table(&lookupTable, NUM_DIRECTIONS, sizeOfCMatrix, &matrix, &labels, ALPHA);
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
        eMat[i] = (int *)malloc(sizeof(int) * sizeOfEMatrix);
    }
    for (int i = 0; i < sizeOfEMatrix; i++)
    {
        for (int j = 0; j < sizeOfEMatrix; j++)
            eMat[i][j] = 0;
    }

    int done = create_e_matrix(&lookupTable, sizeOfCMatrix, sizeOfEMatrix, &eMat, input, output, NUM_DIRECTIONS, VEC_HIST_LEN);

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

int print_c_matrix_to_file(int sizeOfCMatrix, char ***ptrToLabels, double ***ptrToMatrix, FILE *output_file)
{
    char **labels = *ptrToLabels;
    double **matrix = *ptrToMatrix;
    int row, col, top;
    fprintf(output_file, "Co-occurrence Matrix: \n");
    fprintf(output_file, "            ");
    for (top = 0; top < sizeOfCMatrix; top++)
    {
        fprintf(output_file, "%12s", labels[top]);
        if (top == sizeOfCMatrix - 1)
        {
            fprintf(output_file, "\n");
        }
    }

    for (row = 0; row < sizeOfCMatrix; row++)
    {
        fprintf(output_file, "%12s", labels[row]);
        for (col = 0; col < sizeOfCMatrix; col++)
        {
            fprintf(output_file, "%12.0f", matrix[row][col]);
            if (col == sizeOfCMatrix - 1)
            {
                fprintf(output_file, "\n");
            }
        }
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