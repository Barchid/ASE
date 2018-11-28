# $Id: Makefile,v 1.2 2009/11/17 10:02:37 marquet Exp $
##############################################################################

ROOTDIR=/home/enseign/ASE

CC	=  gcc
CFLAGS	=  -m32 -Wall -Werror -ansi -pedantic
CFLAGS  += -std=iso9899:1999
CFLAGS  += -g
LIBDIR  =  $(ROOTDIR)/lib
INCDIR  =  $(ROOTDIR)/include
CFLAGS  += -I$(INCDIR)
LIBS    = -lhardware

###------------------------------
### Main targets 
###------------------------------------------------------------
BINARIES= mi_kernel
OBJECTS	= $(addsuffix .o,\
	   mi_kernel mi_user)
all: $(BINARIES) $(OBJECTS)


###------------------------------
### Binaries
###------------------------------------------------------------
mi_user : mi_user.o $(OBJECTS)
mi_kernel : mi_kernel.o $(OBJECTS)
% : %.o
	$(CC) $(CFLAGS) -o $@ $^ -L$(LIBDIR) $(LIBS)

###------------------------------
### #include dependences 
###------------------------------------------------------------
# you may fill these lines with "make depend"
mi_user.o: mi_user.c mi_user.h mi_syscall.h hw.h hardware.h
mi_kernel.o: mi_kernel.c mi_user.h mi_syscall.h hw.h hardware.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o $(BINARIES)
realclean: clean 
depend : 
	$(CC) $(CFLAGS) -MM $(INCDIR) *.c 
