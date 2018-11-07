ROOTDIR=/home/enseign/ASE

ALL = mknfs dfs mkvol dvol
CC	= gcc
CFLAGS	= -m32 -Wall -pedantic 
CFLAGS  += -g
INCS    = -I$(ROOTDIR)/include
LIBS    = -L$(ROOTDIR)/lib -lhardware

###-----------------------------	-------------------------------
### Main rules 
###------------------------------------------------------------

current.o : current.c current.h
	$(CC) $(CFLAGS) $(INCS) -c $<

drive.o : drive.c drive.h hw.h
	$(CC) $(CFLAGS) $(INCS) -c $<

mbr.o: mbr.c mbr.h drive.h hw.h
	$(CC) $(CFLAGS) $(INCS) -c $<

#### exécutables
mkvol.o: mkvol.c drive.h mbr.h
	$(CC) $(CFLAGS) $(INCS) -c $<

mkvol: mkvol.o drive.o mbr.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

dvol.o: dvol.c drive.h mbr.h
	$(CC) $(CFLAGS) $(INCS) -c $<

dvol: dvol.o drive.o mbr.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

mknfs.o: mknfs.c drive.h mbr.h current.h
	$(CC) $(CFLAGS) $(INCS) -c $<

mknfs: mknfs.o drive.o mbr.o current.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

dfs.o: dfs.c drive.h mbr.h current.h
	$(CC) $(CFLAGS) $(INCS) -c $<

dfs: dfs.o drive.o mbr.o current.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

compile: $(ALL)

clean:
	rm -f *.o *.s $(ALL)