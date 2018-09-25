ALL = try_mul display_stack

compile: $(ALL)

try.o : try.c try.h
	gcc -c $<

try_mul.o : try_mul.c try.h
	gcc -c $<

try_mul: try_mul.o try.o
	gcc $^ -o $@

display_stack: display_stack.c
	gcc $^ -o $@

execNumbers: try_mul
	./try_mul

displayStack: display_stack
	./display_stack

clean:
	rm -f *.o *.s $(ALL)