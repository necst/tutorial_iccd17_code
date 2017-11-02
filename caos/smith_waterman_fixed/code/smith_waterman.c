#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <limits.h>

#include <time.h>

// directions codes
static const int CENTER = 0;
static const int NORTH = 1;
static const int NORTH_WEST = 2;
static const int WEST = 3;

// scores used for Smith Waterman similarity computation
static const short GAP_i = -1;
static const short GAP_d = -1;
static const short MATCH = 2;
static const short MISS_MATCH = -1;

// error codes
static const short SUCCESS = 0;
static const short ERROR_PARAMS = -1;

#define N 128
#define M 2048

/**
 * return a random number between 0 and limit inclusive.
 */
int rand_lim(int limit) {
    int divisor = RAND_MAX / (limit + 1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}

/**
 * fill a string with random letters.
 */
void fill_random(char* string, int length){
	static const char possible_letters[] = "ATCG";
	
	string[0] = '-';
	
	int i;
	for(i = 1; i < length; i++){
		int random_num = rand_lim(3);
		string[i] = possible_letters[random_num];
	}
}

/**
 * print a matrix.
 */
void print_matrix(int *matrix) {
	int i = 0;
	printf("Matrix:\n");
	for(i = 0; i < N * M; i++){
		if(i % N == 0) {
			printf("\n");
		}
		printf(" %d ", matrix[i]);
	}
}

/**
 * perform the trace back step of the Smith Waterman algorithm.
 */
void trace_back(
	char string1[N], char string2[M], 
	int max_index, int similarity_matrix[N * M], short direction_matrix[N * M], 
	char string1_out[N], char string2_out[M], int out_start[1]) {

	int index = max_index;
	int i;
	int j;
	
	int s;
	for(s = 0; s < M; s++){
		string1_out[s] = ' ';
		string2_out[s] = ' ';
	}

	s = M - 1;

	char temp_chari, temp_charj, insert1, insert2;
	
	i = index % N; // column 
	j = index / N; // row

	while(similarity_matrix[index] != 0) {
		
		temp_chari = string1[i];
		temp_charj = string2[j];
		insert1 = '?';
		insert2 = '?';

		if(direction_matrix[index] == NORTH) {
			insert1 = '-';
			insert2 = temp_charj;
			//update the index for next turn
			j = j - 1;
			index = index - N;				
		} else if(direction_matrix[index] == WEST){
			insert1 = temp_chari;
			insert2 = '-';
			i--;
			index --;
		} else if(direction_matrix[index] == NORTH_WEST){
			insert1 = temp_chari;
			insert2 = temp_charj;
			i--;
			j--;
			index = j * N + i;
		}

		string1_out[s] = insert1;
		string2_out[s] = insert2;
		s--;
	}

	string1_out[s] = string1[i];
	string2_out[s] = string2[j];

	out_start[0] = s;
}

void compute_matrices(
	char string1[N], char string2[M], 
	int max_index[1], int similarity_matrix[N * M], short direction_matrix[N * M])
{
	//here the real computation starts...
	int index = 0;
	int i = 0;
	int j = 0;
	short dir = CENTER;
	short match = 0;
	int val = 0;
	int north = 0;
	int west = 0;
	int northwest = 0;
	int max_value = 0;
	int test_val = 0;

	max_index[0] = 0;

	for(index = N; index < N * M; index++) {

		dir = CENTER;
		val = 0;

		i = index % N; // column index
		j = index / N; // row index

		if(i == 0) {
			// first column
			west = 0;
			northwest = 0;
		} else {
			
			// all columns but first
			north = similarity_matrix[index - N];
			match = (string1[i] == string2[j] ? MATCH : MISS_MATCH);

			test_val = northwest + match;
			if(test_val > val){
				val = test_val;
				dir = NORTH_WEST;
			}

			test_val = north + GAP_d;
			if(test_val > val){
				val = test_val;
				dir = NORTH;
			}

			test_val = west + GAP_i;
			if(test_val > val){
				val = test_val;
				dir = WEST;
			}

			similarity_matrix[index] = val;
			direction_matrix[index] = dir;
			west = val;
			northwest = north;
			if(val > max_value) {
				max_index[0] = index;
				max_value = val;
			}
		}
	}
}

void compute_smith_waterman(
	char string1[N], char string2[N * M],
	int max_index[1], int similarity_matrix[N * M], short direction_matrix[N * M], 
	char string1_out[N], char string2_out[M], int out_start[1])
{
	compute_matrices(string1, string2, max_index, similarity_matrix, direction_matrix);
	
	trace_back(string1, string2, 
		max_index[0], similarity_matrix, direction_matrix, string1_out, string2_out, out_start);
}

int main(int argc, char** argv) 
{	
	char *commandline_s1 = NULL;
	char *commandline_s2 = NULL;
	bool verbose = false;
	
	int l = 1;
	for(; l < argc; l += 2){
		printf("\n l; %d \n", l);
		if(argv[l][1] == 'v'){
			printf("VERBOSE: ON\n");
			verbose = true;
			l--;
		} else if(argv[l][1] == 's' && l + 1 < argc){
			if(argv[l][2] == '1'){
				commandline_s1 = argv[l+1];
				printf("command line string: %s\n", commandline_s1);
			} else if(argv[l][2] == '2'){
				commandline_s2 = argv[l+1];
				printf("command line string: %s\n", commandline_s2);
			}
		} else {
			printf("Don't really know what you're trying to do....\n");
			return ERROR_PARAMS;
		}
	}
	
	int max_index;
	int out_start;
	char *string1 = (char*) malloc(sizeof(char) * N);
	char *string2 = (char*) malloc(sizeof(char) * M);
	char *string1_out = (char*) malloc(sizeof(char) * M);
	char *string2_out = (char*) malloc(sizeof(char) * M);
	int *similarity_matrix = (int*) malloc(sizeof(int) * N * M);
	short *direction_matrix = (short*) malloc(sizeof(short) * N * M);

	//check what we have...
	if(commandline_s1 == NULL) {
		printf("using random string for 1\n");
		fill_random(string1, N);
	} else {
		printf("using command line string for 1\n");
		strncpy(string1, commandline_s1, N);
	}

	if(commandline_s2 == NULL){
		printf("using random string for 2\n");
		fill_random(string2, M);
	} else {
		printf("using command line string for 2\n");
		strncpy(string2, commandline_s2, M);
	}
  
	printf("filling local arrays...\n");
	int i;
	//fill the local arrays with data and init result array...
	for(i = 0; i < N * M; i++){
		similarity_matrix[i] = 0;
		direction_matrix[i] = 0;
	}
	
	if(verbose) {
		printf("string...\n");
		for(i = 0; i < N; i++){
			printf("%c" , string1[i]);
		}
		
		printf("\n");
		for(i = 0; i < M; i++){
			printf("%c" , string2[i]);
		}
	}
	printf("\n");
		
	compute_smith_waterman(
		string1, string2,
		&max_index, similarity_matrix, direction_matrix, string1_out, string2_out, &out_start);

	
	if(verbose) {
		print_matrix(similarity_matrix);
	}

	printf("The index achieving maximum score is: %d\n", max_index);
	printf("The maximum score is: %d\n", similarity_matrix[max_index]);

	printf("\n Tracing results....\n");

	for(i = out_start; i < M; i++){
		printf("%c", string1_out[i]);
	}
	printf("\n");
	for(i = out_start; i < M; i++){
		printf("%c", string2_out[i]);
	}
	printf("\n");

	printf("END cleaning up...\n");
	
	free(string1);
	free(string2);
	free(similarity_matrix);
	free(direction_matrix);
	free(string1_out);
	free(string2_out);
	
	return SUCCESS;
}
