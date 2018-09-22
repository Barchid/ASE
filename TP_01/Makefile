try_mul: try_mul.o try.o
	gcc -m32 -o try_mul try.o try_mul.o

try.o: try.c
	gcc -m32 -o try.o -c try.c

try_mul.o: try_mul.c try.h
	gcc -m32 -o try_mul.o -c try_mul.c
