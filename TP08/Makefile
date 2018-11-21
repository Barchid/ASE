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
BINARIES= if_pfile if_nfile if_cfile if_dfile adhoc mkvol dvol mknfs dfs
OBJECTS	= $(addsuffix .o,\
	  inode bloc mbr drive tools ifile mount)

all: $(BINARIES) $(OBJECTS)


###------------------------------
### Binaries
###------------------------------------------------------------
if_cfile : if_cfile.o $(OBJECTS)
if_nfile : if_nfile.o $(OBJECTS)
if_dfile : if_dfile.o $(OBJECTS)
if_pfile : if_pfile.o $(OBJECTS)
adhoc: adhoc.o $(OBJECTS) 
mkvol: mkvol.o $(OBJECTS)
dvol: dvol.o $(OBJECTS)
mknfs: mknfs.o $(OBJECTS)
dfs: dfs.o $(OBJECTS)

% : %.o
	$(CC) $(CFLAGS) -o $@ $^ -L$(LIBDIR) $(LIBS)

###------------------------------
### #include dependences 
###------------------------------------------------------------
# you may fill these lines with "make depend"
if_cfile.o: if_cfile.c ifile.h inode.h tools.h mount.h
if_dfile.o: if_dfile.c ifile.h inode.h tools.h mount.h
if_nfile.o: if_nfile.c ifile.h inode.h tools.h mount.h
if_pfile.o: if_pfile.c ifile.h inode.h tools.h mount.h

adhoc.o: adhoc.c mbr.h drive.h bloc.h
mkvol.o: mkvol.c mbr.h drive.h
dvol.o: dvol.c mbr.h drive.h
mknfs.o: mknfs.c mbr.h drive.h bloc.h
dfs.o: dfs.c mbr.h drive.h bloc.h

ifile.o: ifile.c inode.h tools.h ifile.h 
mount.o: mount.c hw.h tools.h 
tools.o: tools.c tools.h

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
