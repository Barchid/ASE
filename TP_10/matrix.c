#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

static void log_state(int i, int n) {
	if(n && i % n == n - 1) {
		printf(".");
		fflush(stdout);
	}
}

void matrix_init(matrix *m) {
	int i,j;
	for(i = 0; i<MATRIX_SIZE; i++) {
		for(j = 0; j<MATRIX_SIZE; j++) {
			(*m)[i][j] = rand();
		}
	}
}

void matrix_add(matrix *dest, matrix *m1, matrix *m2) {
	int i, j;
	for(i = 0; i<MATRIX_SIZE; i++) {
		log_state(i, MATRIX_SIZE / 20);
		for(j = 0; j<MATRIX_SIZE; j++) {
			(*dest)[i][j] = (*m1)[i][j] + (*m2)[i][j];
		}
	}
	printf(" done\n");
}

void matrix_mult(matrix *dest, matrix *m1, matrix *m2) {
	int i, j, k;
	for(i = 0; i<MATRIX_SIZE; i++) {
		log_state(i, MATRIX_SIZE / 20);
		for(j = 0; j<MATRIX_SIZE; j++) {
			int curr_val = 0;
			for(k = 0; k<MATRIX_SIZE; k++) {
				curr_val += (*m1)[i][k] * (*m2)[k][j];
			}
			(*dest)[i][j] = curr_val;
		}
	}
	printf(" done\n");
}

unsigned matrix_checksum(matrix *m) {
	int i, j;
	unsigned int checksum = 0;
	for(i = 0; i<MATRIX_SIZE; i++) {
		for(j = 0; j<MATRIX_SIZE; j++) {
			checksum += (*m)[i][j];
			checksum += checksum >> 16;
			checksum &= 0x0000ffff;
		}
	}
	return checksum;
}

