ROOTDIR=/home/enseign/ASE

CC	= gcc
CFLAGS	= -m32 -Wall -pedantic 
CFLAGS  += -g
INCS    = -I$(ROOTDIR)/include
LIBS    = -L$(ROOTDIR)/lib -lhardware

###------------------------------
### Main rules 
###------------------------------------------------------------

context.o : context.c context.h
	$(CC) $(CFLAGS) $(INCS) -c $<

pingpongpang.o: pingpongpang.c context.h
	$(CC) $(CFLAGS) $(INCS) -c $<

pingpongpang: pingpongpang.o context.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

execPingPongPang: pingpongpang
	./pingpongpang

clean:
	rm -f *.o *.s pingpongpang