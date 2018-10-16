ROOTDIR=/home/enseign/ASE

CC	= gcc
CFLAGS	= -m32 -Wall -pedantic 
CFLAGS  += -g
INCS    = -I$(ROOTDIR)/include
LIBS    = -L$(ROOTDIR)/lib -lhardware

###------------------------------
### Main rules 
###------------------------------------------------------------

sem.o : sem.c sem.h hw.h
	$(CC) $(CFLAGS) $(INCS) -c $<

prodcons.o: prodcons.c sem.h
	$(CC) $(CFLAGS) $(INCS) -c $<

prodcons: prodcons.o sem.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

execProdCons: prodcons
	./prodcons

clean:
	rm -f *.o *.s prodcons