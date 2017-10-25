#include "matrices.h"
#include "parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED 0
#define GREEN 1
#define BLUE 2

void print_matrix(short mat[HEIGHT][WIDTH]) {
	int i, j;

	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			printf("%3d ", mat[i][j]);
		}
		printf("\n");
	}
}

//read a single channel from image
void load_ppm(char *filename, unsigned char mat[HEIGHT][WIDTH], int rgb) {
	int i, j, is_rgb;
	char mode[2];
	unsigned int buf[3], pix;

	FILE *input = fopen(filename, "r");
	if (input == NULL) {
		printf("Error in loading image %s\n", filename);
		exit(1);
	}

	fscanf(input, "%s %*d %*d %*d\n", mode);
	if (strcmp(mode, "P2") == 0)
		is_rgb = 0;
	else if (strcmp(mode, "P3") == 0)
		is_rgb = 1;
	else {
		printf("Image error, mode not present (either P2 or P3)\n");
		exit(1);
	}
	//loading pixels
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {

			fscanf(input, "%u", &buf[RED]);
			if (is_rgb) {
				fscanf(input, "%u", &buf[GREEN]);
				fscanf(input, "%u", &buf[BLUE]);
			}

			if (is_rgb)
				pix = buf[rgb];
			else
				pix = buf[RED];

			mat[i][j] = (unsigned char) pix;
		}
	}

	fclose(input);
}

void save_ppm(char *filename, unsigned char mat[HEIGHT][WIDTH]) {
	int i, j;
	FILE *output = fopen(filename, "w");
	if (output == NULL) {
		printf("Error in opening file %s in write mode\n", filename);
		exit(1);
	}

	fprintf(output, "P2 %d %d 255\n", WIDTH, HEIGHT);

	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			fprintf(output, "%3u ", (unsigned int) mat[i][j]);
		}
		fprintf(output, "\n");
	}

	fclose(output);
}

void match_filter(unsigned char tmp[HEIGHT][WIDTH],
		unsigned char input[HEIGHT][WIDTH]) {

	unsigned int max, sum;

	static unsigned char in[HEIGHT][WIDTH];
	static unsigned char out[HEIGHT][WIDTH];
	unsigned int i, j, theta, row, col;

	memcpy(in, input, sizeof(unsigned char) * HEIGHT * WIDTH);

	for (i = CENTER; i < HEIGHT - (CENTER - 1 + KERN_SIZE % 2); i++) {
		for (j = CENTER; j < WIDTH - (CENTER - 1 + KERN_SIZE % 2); j++) {

			out[i][j] = 0;
			max = 0;
			for (theta = 0; theta < NUM_FILTERS; theta++) {
				sum = 0;
				for (row = 0; row < KERN_SIZE; row++) {
					for (col = 0; col < KERN_SIZE; col++) {
						sum += ((unsigned int) coeffs[theta][row][col]
								* (unsigned int) in[i - CENTER + row][j - CENTER
										+ col]);
					}
				}
				if (sum > max)
					max = sum;
			}

			if (max / DIVISOR >= THRESHOLD && max / DIVISOR < 255)
				out[i][j] = 255 - max / DIVISOR;
			else if (max / DIVISOR >= 255)
				out[i][j] = 0;
			else
				out[i][j] = 255;
		}
	}

	memcpy(tmp, out, sizeof(unsigned char) * HEIGHT * WIDTH);
}


int main(int argc, char * argv[]) {
	int i, j;

	static unsigned char input[HEIGHT][WIDTH];
	static unsigned char tmp[HEIGHT][WIDTH];

	char *input_name;
	char output_name[] = "software_out.ppm";

	printf("Filling matrix...\n");
	if (argc > 1) {
		input_name = argv[1];
		load_ppm(input_name, input, GREEN);
	} else {
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				input[i][j] = (unsigned char) (rand() % 256);
				tmp[i][j] = 0;
			}
		}
	}

	printf("Done!\n");

	printf("Applying match filter...\n");
	match_filter(tmp, input);
	printf("Done!\n");

	if (argc > 1) {
		printf("Saving output...\n");
		save_ppm(output_name, tmp);
	}

	printf("Exiting!\n");
	return 0;

}

