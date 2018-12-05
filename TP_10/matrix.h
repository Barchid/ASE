#ifndef _MATRIX_H_
#define _MATRIX_H_

#define MATRIX_SIZE 400
#define MATRIX_ADD 0
#define MATRIX_MUL 1

typedef unsigned int matrix[MATRIX_SIZE][MATRIX_SIZE];

extern void matrix_init(matrix *m);
extern void matrix_add(matrix *dest, matrix *m1, matrix *m2);
extern void matrix_mult(matrix *dest, matrix *m1, matrix *m2);
extern unsigned matrix_checksum(matrix *m);

#endif

