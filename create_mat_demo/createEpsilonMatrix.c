#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
    1.	Take a past and future vector from the raw data. 
        Find the compressed vector for each 
        (DO NOT RECODE-- REUSE THE CODE KAITLYN HAS ALREADY DONE). 
    2.	Take the past compressed vector, and look it up in the array of luRow 
        structs. Find the corresponding past epsilon vector.
    3.	Take the future compressed vector, and look it up in the array of luRow 
        structs. Find the corresponding future epsilon vector.
    4.	Go to the epsilon matrix. Increment the cell where the past epsilon vector 
        row and future epsilon vector column intersect.
*/

int BUFF_SIZE = 128;
char *DEF_OUTPUT_FILE = "output";
int debug = 0;

struct luRow
{
    char epsilon[5];
    int vector[4];
};

//int writeMatrixToFile(struct luRow *lookupTable, int sizeofLookupTable, int mat[][6], FILE *output_file);
int getEpsilonPos(char *epString, struct luRow *lookupTable, int sizeofLookupTable);
int createMatrix(struct luRow *lookupTable, int sizeofLookupTable, int a, int b, int vectors[a][b]);

int main()
{
    FILE *my_file = fopen("test.csv", "r");
    struct luRow testTable[16];

    for (int count = 0; count < (sizeof(testTable) / sizeof(testTable[0])); count++)
    {
        int got = fscanf(my_file, "%c%c%c%c%c, ",
                         &testTable[count].epsilon[0], &testTable[count].epsilon[1], &testTable[count].epsilon[2], &testTable[count].epsilon[3], &testTable[count].epsilon[4]);
        int got2 = fscanf(my_file, "{%d,%d,%d,%d}\n",
                          &testTable[count].vector[0], &testTable[count].vector[1], &testTable[count].vector[2], &testTable[count].vector[3]);
        if (got != 5)
        {
            break;
        }
        if (got2 != 4)
        {
            break;
        }
        if (debug)
        {
            printf("%s, ", testTable[count].epsilon);
            for (int i = 0; i < 4; i++)
            {
                printf("%d", testTable[count].vector[i]);
            }
            printf("\n");
        }
    }
    fclose(my_file);
    const int sizeofLookupTable = sizeof(testTable) / sizeof(testTable[0]);

    int vectors[][4] = {
        {0, 0, 0, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 2},
        {0, 0, 0, 2},
        {0, 0, 0, 3},
        {0, 0, 0, 3},
        {0, 0, 0, 4},
        {0, 0, 0, 4},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 2, 0},
        {0, 0, 2, 0}, //<------------ 12
        {0, 0, 0, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 2},
        {0, 0, 0, 2},
        {0, 0, 0, 3},
        {0, 0, 0, 3},
        {0, 0, 0, 4},
        {0, 0, 0, 4},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 2, 0},
        {0, 0, 2, 0}, //<------------ 12
        {0, 0, 0, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 2},
        {0, 0, 0, 2},
        {0, 0, 0, 3},
        {0, 0, 0, 3},
        {0, 0, 0, 4},
        {0, 0, 0, 4},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 2, 0},
        {0, 0, 2, 0}};
    int totalCells = sizeof(vectors) / sizeof(int);
    int numCols = sizeof(vectors[0]) / sizeof(int);
    int numRows = totalCells / numCols;
    if (debug)
    {
        printf("Total fields: %d\n", totalCells);
        printf("Number of rows: %d\n", numRows);
        printf("Number of columns: %d\n", numCols);
    }
    createMatrix(testTable, sizeofLookupTable, numRows, numCols, vectors);
}

int createMatrix(struct luRow *lookupTable, int sizeofLookupTable, int numRows, int numCols, int vectors[numRows][numCols])
{
    // Create epsilon matrix (sizeofLookupTable x sizeofLookupTable)
    int eMat[sizeofLookupTable][sizeofLookupTable];
    for (int i = 0; i < sizeofLookupTable; i++)
    {
        for (int j = 0; j < sizeofLookupTable; j++)
        {
            eMat[i][j] = 0;
        }
    }

    int i, j, k, m;
    char *pastE;
    char *futureE;
    int pastEPos, futureEPos;
    // For each compressed vector
    for (i = 0; i < numRows; i++)
    {
        // For each entry in the lookup table
        for (j = 0; j < sizeofLookupTable; j++)
        {
            // For each coordinate in a single compressed vector (4)
            for (k = 0; k < 4; k++)
            {
                // If the coordinate of the current vector matches
                //      that of the current lookup table entry, --continue--
                // Otherwise, move on to the next entry in the lookup table
                if (vectors[i][k] == lookupTable[j].vector[k])
                {
                    if (k == 3)
                    {
                        if (i % 2 == 0)
                        {
                            pastEPos = getEpsilonPos(lookupTable[j].epsilon, lookupTable, sizeofLookupTable);
                        }
                        else
                        {
                            futureEPos = getEpsilonPos(lookupTable[j].epsilon, lookupTable, sizeofLookupTable);
                            eMat[pastEPos][futureEPos]++;
                        }
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }

    // Write eMat to output file
    FILE *output_file;
    //writeMatrixToFile(lookupTable, sizeofLookupTable, eMat, output_file);
    // Open output file
    char filename[BUFF_SIZE];
    strcpy(filename, DEF_OUTPUT_FILE);
    strcat(filename, ".txt");
    output_file = fopen(filename, "w");

    //Displaying array elements
    int row, col, top;
    fprintf(output_file, "Epsilon matrix:\n");
    fprintf(output_file, "      ");
    for (top = 0; top < sizeofLookupTable; top++)
    {
        fprintf(output_file, "%s ", lookupTable[top].epsilon);
        if (top == sizeofLookupTable - 1)
        {
            fprintf(output_file, "\n");
        }
    }
    for (row = 0; row < sizeofLookupTable; row++)
    {
        fprintf(output_file, "%s ", lookupTable[row].epsilon);
        for (col = 0; col < sizeofLookupTable; col++)
        {
            fprintf(output_file, "  %d   ", eMat[row][col]);
            if (col == sizeofLookupTable - 1)
            {
                fprintf(output_file, "\n");
            }
        }
    }
    fclose(output_file);
    return 1;
}

int getEpsilonPos(char *epString, struct luRow *lookupTable, int sizeofLookupTable)
{
    for (int m = 0; m < sizeofLookupTable; m++)
    {
        if (!strcmp(epString, lookupTable[m].epsilon))
        {
            return m;
        }
    }
    return -1;
}

// int writeMatrixToFile(struct luRow *lookupTable, int sizeofLookupTable, int mat[][6], FILE *output_file)
// {
//     // Open output file
//     char filename[BUFF_SIZE];
//     strcpy(filename, DEF_OUTPUT_FILE);
//     strcat(filename, ".txt");
//     output_file = fopen(filename, "w");

//     //Displaying array elements
//     int row, col, top;
//     fprintf(output_file, "Epsilon matrix:\n");
//     fprintf(output_file, "      ");
//     for (top = 0; top < sizeofLookupTable; top++)
//     {
//         fprintf(output_file, "%s ", lookupTable[top].epsilon);
//         if (top == sizeofLookupTable - 1)
//         {
//             fprintf(output_file, "\n");
//         }
//     }
//     for (row = 0; row < sizeofLookupTable; row++)
//     {
//         fprintf(output_file, "%s ", lookupTable[row].epsilon);
//         for (col = 0; col < sizeofLookupTable; col++)
//         {
//             fprintf(output_file, "  %d   ", mat[row][col]);
//             if (col == sizeofLookupTable - 1)
//             {
//                 fprintf(output_file, "\n");
//             }
//         }
//     }
//     return 1;
// }
