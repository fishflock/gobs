#include <stdio.h>
#include <string.h>

int BUFF_SIZE = 128;
int DEF_HISTORY_LEN = 5;
int DEF_DIR = 4;
char * DEF_INPUT_FILE = "sample";
char * DEF_OUTPUT_FILE = "compressvector";

int main() {
	FILE * input_file;
	FILE * output_file;
	char filename[BUFF_SIZE];
	int h_len;
	int dir = DEF_DIR;
	char buffer[BUFF_SIZE];

	// Open input file
	strcpy(filename, DEF_INPUT_FILE);
	printf("open file(%s)=", DEF_INPUT_FILE);
	fgets(buffer, BUFF_SIZE, stdin);
	sscanf(buffer, "%s", filename);
	strcat(filename, ".csv");
	input_file = fopen(filename, "r");
	
	// Open output file
	strcpy(filename, DEF_OUTPUT_FILE);
	printf("open file(%s)=", DEF_OUTPUT_FILE);
	fgets(buffer, BUFF_SIZE, stdin);
	sscanf(buffer, "%s", filename);
	strcat(filename, ".txt");
	output_file = fopen(filename, "w");

	// Set history length
	h_len = DEF_HISTORY_LEN;
	printf("history length(%d)=", DEF_HISTORY_LEN);
	fgets(buffer, BUFF_SIZE, stdin);
	sscanf(buffer, "%d", &h_len);
	
	// Compression vector generation test
	// Size of compression vector is dir
	// Order: past, future, repeat
	int comp_vector[dir];
	for(int j = 0; j < dir; j++) comp_vector[j] = 0;
	int v;
	
	for(int k = 0; k < 2 * h_len; k++) {
	fseek(input_file, k, SEEK_SET);
	int i = 0;
	int pf = 0; // past = 0, future = 1
	while(fread(&v, 1, 1, input_file)) {
		int v2 = v - '0';
		if((v2 > 0) && (v2 < dir + 1)) { // if v2 is a valid character (1-4)
			comp_vector[v2 - 1] += 1;
			i = (i + 1) % h_len; // move i to next position
			if(i == 0) {
				for(int j = 0; j < dir; j++) {
					char v3[2];
					sprintf(v3, "%d", comp_vector[j]);
					fwrite(v3, 1, 1, output_file);
					//printf("%s", v3);
				}
				if(pf == 0) {
					fwrite(":", 1, 1, output_file);
					//printf(":");
				}
				if(pf == 1) {
					fwrite("\n", 1, 1, output_file);
					//printf("\n");
				}
				pf = (pf + 1) % 2; // move pf to next type
				for(int j = 0; j < dir; j++) comp_vector[j] = 0; // clear
			}
		}
	}
	if(pf == 1) {
		fwrite("\n", 1, 1, output_file);
		//printf("\n");
	}
	}
	
	fclose(input_file);
	fclose(output_file);
}

