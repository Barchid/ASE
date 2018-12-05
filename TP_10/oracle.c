#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hw_config.h"
#include "matrix.h"

int main() {
	int operation;
	unsigned timestamp;
	unsigned prev_checksum;
	unsigned new_checksum;
	matrix matrix1;
	matrix matrix2;
	matrix matrix3;
	char line[1024];

	while(!feof(stdin)) {
		fgets(line, sizeof(line), stdin);
	}
	
	if(sscanf(line, "timestamp: 0x%x, operation: %d, checksum: 0x%x", &timestamp, &operation, &prev_checksum) == 0) {
		fprintf(stderr, "[oracle] Invalid input\n");
		exit(1);
	}
		
	srand(timestamp);
	
	printf("[Starting oracle]\n");

	printf("timestamp: 0x%04x, ", timestamp);
	printf("operation: %d, ", operation);
	printf("checksum: 0x%04x\n", prev_checksum);
	/* init matrices */
	printf("initializing matrices\n");
	matrix_init(&matrix1);
	matrix_init(&matrix2);
	switch(operation) {
		case 0: /* add matrices */
			printf("adding VM matrices ");fflush(stdout);
			matrix_add(&matrix3, &matrix1, &matrix2);
			break;
		case 1: /* multiply matrices */
			printf("multiplying matrices ");fflush(stdout);
			matrix_mult(&matrix3, &matrix1, &matrix2);
			break;
		default:
			break;
	}
	new_checksum = matrix_checksum(&matrix3);
	printf("new checksum: 0x%04x [%s]\n", new_checksum, new_checksum == prev_checksum ? "ok" : "ko");
	
	return 0;
}

