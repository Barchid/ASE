ALL = try_mul display_stack

compile: $(ALL)

try.o : try.c try.h
	gcc -m32 -c $<

try_mul.o : try_mul.c try.h
	gcc -m32 -c $<

try_mul: try_mul.o try.o
	gcc -m32 $^ -o $@

display_stack: display_stack.c
	gcc -m32 $^ -o $@

execNumbers: try_mul
	./try_mul

displayStack: display_stack
	./display_stack

clean:
	rm -f *.o *.s $(ALL)