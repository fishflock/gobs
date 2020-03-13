#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_c_vectors.h"
#include "create_c_matrix.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int BUFF_SIZE = 128;
char * DEF_INPUT_FILE = "sample";
char * DEF_OUTPUT_FILE = "compressvector2";
char * DEF_INPUT_FILEPATH = "input/";
char * DEF_OUTPUT_FILEPATH = "output/";

int main() {
	FILE * input_file;
	FILE * output_file;
	char filename[BUFF_SIZE];
	char buffer[BUFF_SIZE];
        struct stat st = {0};

	// Open input file
	strcpy(filename, DEF_INPUT_FILE);
	printf("open file(%s)=", DEF_INPUT_FILE);
	fgets(buffer, BUFF_SIZE, stdin);
	sscanf(buffer, "%s", filename);
	strcat(filename, ".csv");
        strcpy(buffer, DEF_INPUT_FILEPATH);
        if (stat(buffer, &st) == -1) {
            perror("stat");
            exit(EXIT_FAILURE);
        }
	strcat(buffer, filename);
        if ((input_file = fopen(buffer, "r")) == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
	
	// Open output file
	strcpy(filename, DEF_OUTPUT_FILE);
	printf("open file(%s)=", DEF_OUTPUT_FILE);
	fgets(buffer, BUFF_SIZE, stdin);
	sscanf(buffer, "%s", filename);
	strcat(filename, ".txt");
        strcpy(buffer, DEF_OUTPUT_FILEPATH);
        if (stat(buffer, &st) == -1)
            mkdir(buffer, 0777);
	strcat(buffer, filename);
	if ((output_file = fopen(buffer, "w")) == NULL) {
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
        char labels[size][dir + 1];
        int matrix[size][size];
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) matrix[i][j] = 0;
        }

        int x = create_c_matrix_default(size, labels, matrix, input_file);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) printf("%d ", matrix[i][j]);
            printf("\n");
        }

        printf("\nlabels:\n");
        for (int i = 0; i < size; i++) {
            printf("%d: %s\n", i, labels[i]);
        }

	fclose(input_file);
	fclose(output_file);
}

