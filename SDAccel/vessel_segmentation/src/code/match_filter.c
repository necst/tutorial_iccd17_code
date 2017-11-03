#include "runtime/runtime.h"

#include "matrices.h"
#include "parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED 0
#define GREEN 1
#define BLUE 2

void print_matrix(short mat[HEIGHT][WIDTH])
{
    int i, j;

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }
}

//read a single channel from image
void load_ppm(char *filename, unsigned char mat[HEIGHT][WIDTH], int rgb)
{
    int i, j, is_rgb;
    char mode[2];
    unsigned int buf[3], pix;

    FILE *input = fopen(filename, "r");
    if (input == NULL) {
        printf("Error in loading image %s\n", filename);
        exit(1);
    }

    fscanf(input, "%s %*d %*d %*d\n", mode);
    if (strcmp(mode, "P2") == 0) {
        is_rgb = 0;
    } else if (strcmp(mode, "P3") == 0) {
        is_rgb = 1;
    } else {
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

            if (is_rgb) {
                pix = buf[rgb];
            } else {
                pix = buf[RED];
            }

            mat[i][j] = (unsigned char) pix;
        }
    }

    fclose(input);
}

void save_ppm(char *filename, unsigned char mat[HEIGHT][WIDTH])
{
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




void match_filter(
	unsigned char out[1080][1440],
	unsigned char in[1080][1440])
{
	cl_mem d_out = xcl_malloc(_caos_runtime.world, CL_MEM_READ_WRITE,  sizeof(unsigned char) * 1555200);
	cl_mem d_in = xcl_malloc(_caos_runtime.world, CL_MEM_READ_WRITE,  sizeof(unsigned char) * 1555200);

	xcl_memcpy_to_device(_caos_runtime.world, d_out, out, sizeof(unsigned char) * 1555200);
	xcl_memcpy_to_device(_caos_runtime.world, d_in, in, sizeof(unsigned char) * 1555200);

	xcl_set_kernel_arg(_caos_runtime.match_filter_caos_kernel, 0, sizeof(cl_mem), &d_out);
	xcl_set_kernel_arg(_caos_runtime.match_filter_caos_kernel, 1, sizeof(cl_mem), &d_in);

	cl_event enqueue_kernel;
	int err = clEnqueueTask(_caos_runtime.world.command_queue, _caos_runtime.match_filter_caos_kernel, 0, NULL, &enqueue_kernel);
	if (err) { 
	printf("Error: Failed to execute kernel! %d\nTest failed", err);
	exit(-1);
	}

	clWaitForEvents(1, &enqueue_kernel);

	xcl_memcpy_from_device(_caos_runtime.world, out, d_out, sizeof(unsigned char) * 1555200);
	xcl_memcpy_from_device(_caos_runtime.world, in, d_in, sizeof(unsigned char) * 1555200);
}



int main(int argc, char * argv[]) 
{
	caos_init_runtime(&argc, &argv);
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

