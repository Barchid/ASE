# $Id: Makefile 102 2009-11-03 13:14:39Z simon_duquennoy $
##############################################################################
ROOTDIR = /home/enseign/ASE/
LIBNAME = hardware

CC	= gcc
CFLAGS	= -Wall -ansi -pedantic
LIBDIR  = $(ROOTDIR)
INCDIR  = -I$(ROOTDIR)/include
LIBS    = -L$(ROOTDIR)/lib -l$(LIBNAME)

###------------------------------
### Main targets 
###------------------------------------------------------------
BINARIES= mmu_manager oracle

all: $(BINARIES)

###------------------------------
### Main rules 
###------------------------------------------------------------

%.o: %.c
	$(CC) $(CFLAGS) -c $< $(INCDIR)

%: %.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

Makefile.depends:
	$(CC) -MM *.c $(INCDIR) > Makefile.depends

include Makefile.depends

mmu_manager: mmu_manager.o user_process.o matrix.o
oracle: oracle.o matrix.o

###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o $(BINARIES) Makefile.depends
realclean: clean 
	$(RM) vdiskA.bin vdiskB.bin
