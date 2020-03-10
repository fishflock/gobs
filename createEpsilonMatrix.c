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
int DEF_HISTORY_LEN = 5;
int DEF_DIR = 4;
char *DEF_OUTPUT_FILE = "output";

struct luRow
{
    char epsilon[5];
    int vector[4];
};

int main()
{
    // Lookup table
    struct luRow entry1 = {"e0001", {2, 2, 1, 0}};
    struct luRow entry2 = {"e0002", {1, 0, 2, 2}};
    struct luRow entry3 = {"e0001", {1, 2, 2, 0}};
    struct luRow entry4 = {"e0002", {2, 0, 1, 2}};
    struct luRow entry5 = {"e0010", {0, 2, 3, 0}};
    struct luRow entry6 = {"e0020", {3, 0, 0, 2}};
    struct luRow lookupTable[6] = {entry1, entry2, entry3, entry4, entry5, entry6};
    const int sizeofLookupTable = sizeof(lookupTable) / sizeof(lookupTable[0]);
    printf("\n\n------ size of lookup table: %d \n", sizeofLookupTable);

    int vectors[6][4] = {
        {2, 2, 1, 0}, {1, 0, 2, 2}, {1, 2, 2, 0}, {2, 0, 1, 2}, {0, 2, 3, 0}, {3, 0, 0, 2}};

    // numRows*numCol
    int totalCells = sizeof(vectors) / sizeof(int);
    int numCols = sizeof(vectors[0]) / sizeof(int);
    int numRows = totalCells / numCols;
    printf("Total fields: %d\n", totalCells);
    printf("Number of rows: %d\n", numRows);
    printf("Number of columns: %d\n", numCols);

    // Create epsilon matrix (sizeofLookupTable x sizeofLookupTable)
    int eMat[sizeofLookupTable][sizeofLookupTable] = {{0}};

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
                    //printf("\nvectors[i][k]ector: %d", vectors[i][k]);
                    //printf("\nlookupTable[j].vector[k]: %d", lookupTable[j].vector[k]);
                    if (k == 3)
                    {
                        if (i % 2 == 0)
                        {
                            pastE = lookupTable[j].epsilon;
                            printf("\nsetting past epsilon to : %s", lookupTable[j].epsilon);
                            for (m = 0; m < sizeofLookupTable; m++)
                            {
                                if (!strcmp(pastE, lookupTable[m].epsilon))
                                {
                                    pastEPos = m;
                                    printf("\n  @ position : %d", pastEPos);
                                    break;
                                }
                            }
                        }
                        else
                        {
                            futureE = lookupTable[j].epsilon;
                            printf("\nsetting future ep to : %s", lookupTable[j].epsilon);
                            for (m = 0; m < sizeofLookupTable; m++)
                            {
                                if (!strcmp(futureE, lookupTable[m].epsilon))
                                {
                                    futureEPos = m;
                                    printf("\n  @ position : %d\n\n", futureEPos);

                                    eMat[pastEPos][futureEPos]++;

                                    break;
                                }
                            }
                        }
                        //printf("\n-------->found a match to %s\n\n", lookupTable[j].epsilon);
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }

    int row, col, top;
    //Displaying array elements
    printf("Epsilon matrix:\n");
    printf("      ");
    for (top = 0; top < sizeofLookupTable; top++)
    {
        printf("%s ", lookupTable[top].epsilon);
        if (top == sizeofLookupTable - 1)
        {
            printf("\n");
        }
    }
    for (row = 0; row < sizeofLookupTable; row++)
    {
        printf("%s ", lookupTable[row].epsilon);
        for (col = 0; col < sizeofLookupTable; col++)
        {
            printf("  %d   ", eMat[row][col]);
            if (col == sizeofLookupTable - 1)
            {
                printf("\n");
            }
        }
    }

    FILE *output_file;
    char filename[BUFF_SIZE];
    int h_len;
    int dir = DEF_DIR;
    char buffer[BUFF_SIZE];
    char temp[BUFF_SIZE];

    // Open output file
    strcpy(filename, DEF_OUTPUT_FILE);
    printf("open file(%s)=", DEF_OUTPUT_FILE);
    fgets(buffer, BUFF_SIZE, stdin);
    sscanf(buffer, "%s", filename);
    strcat(filename, ".txt");
    output_file = fopen(filename, "w");

    //Displaying array elements
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
}
