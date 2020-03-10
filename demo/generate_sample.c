#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int MAX_LEN = 16;
char * FILEPATH = "input/";
char * DEF_FILENAME = "sample";
int DEF_COUNT = 200;
int DEF_DIR = 4;

int main(void) {
	char buffer[128];
	char filename[MAX_LEN];
	int count = DEF_COUNT;
	int dir = DEF_DIR;
        struct stat st = {0};
	srand(time(NULL));
	
	// Get parameters (or default)
	strcpy(filename, DEF_FILENAME);

	printf("filename(%s)=", DEF_FILENAME);
	fgets(buffer, MAX_LEN, stdin);
	sscanf(buffer, "%s", filename);
	
	printf("count(%d)=", DEF_COUNT);
	fgets(buffer, MAX_LEN, stdin);
	sscanf(buffer, "%d", &count);
	
	printf("directions(%d)=", DEF_DIR);
	fgets(buffer, MAX_LEN, stdin);
	sscanf(buffer, "%d", &dir);

	// Open file
	char * csv_postfix = ".csv";
        strcpy(buffer, FILEPATH);
        if (stat(buffer, &st) == -1)
            mkdir(buffer, 0777);
        strcat(buffer, filename);
	strcat(buffer, csv_postfix);

	FILE * output;
	if (!(output = fopen(buffer, "w"))) {
		perror("Open file");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < count; i++) {
		int v = (rand() % 4) + 1;
		fprintf(output, "%d,\n", v);	
	}

	if(!(fclose(output) == 0)) {
		perror("Open file");
		exit(EXIT_FAILURE);
	}

	printf("Complete\n");
	exit(EXIT_SUCCESS);
}
