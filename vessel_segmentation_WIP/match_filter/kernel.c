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
