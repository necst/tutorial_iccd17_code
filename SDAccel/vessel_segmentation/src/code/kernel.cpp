
#include "matrices.h"
#include "parameters.h"

extern "C" {

void match_filter_caos_kernel(unsigned char out[HEIGHT][WIDTH],
                  unsigned char in[HEIGHT][WIDTH])
{
#pragma HLS INTERFACE m_axi port=out 
#pragma HLS INTERFACE s_axilite port=out bundle=control
#pragma HLS INTERFACE m_axi port=in 
#pragma HLS INTERFACE s_axilite port=in bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control


    int max, sum;
    unsigned int i, j, theta, row, col;

    for (i = CENTER; i < HEIGHT - (CENTER - 1 + KERN_SIZE % 2); i++) {
        for (j = CENTER; j < WIDTH - (CENTER - 1 + KERN_SIZE % 2); j++) {

            out[i][j] = 0;
            max = 0;
            for (theta = 0; theta < NUM_FILTERS; theta++) {
                sum = 0;
                for (row = 0; row < KERN_SIZE; row++) {
                    for (col = 0; col < KERN_SIZE; col++) {
                        sum += ((int) coeffs[theta][row][col]
                                * (unsigned int) in[i - CENTER + row][j - CENTER + col]);
                    }
                }
                if (sum > max) {
                    max = sum;
                }
            }

            if (max / DIVISOR >= THRESHOLD && max / DIVISOR < 255) {
                out[i][j] = 255 - max / DIVISOR;
            } else if (max / DIVISOR >= 255) {
                out[i][j] = 0;
            } else {
                out[i][j] = 255;
            }
        }
    }
}

}
