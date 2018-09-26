context.o : context.c context.h
	gcc -m32 -c $<

pingpong.o: pingpong.c context.h
	gcc -m32 -c $<

pingpong: pingpong.o context.o
	gcc -m32 $^ -o $@

clean:
	rm -f *.o *.s pingpong

execPingPong: pingpong
	./pingpong