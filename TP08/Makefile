ROOTDIR=/home/enseign/ASE

ALL = mknfs dfs mkvol dvol adhoc
CC	= gcc
CFLAGS	= -m32 -Wall -pedantic 
CFLAGS  += -g
INCS    = -I$(ROOTDIR)/include
LIBS    = -L$(ROOTDIR)/lib -lhardware

###-----------------------------	-------------------------------
### Main rules 
###------------------------------------------------------------

bloc.o : bloc.c bloc.h
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

mknfs.o: mknfs.c drive.h mbr.h bloc.h
	$(CC) $(CFLAGS) $(INCS) -c $<

mknfs: mknfs.o drive.o mbr.o bloc.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

dfs.o: dfs.c drive.h mbr.h bloc.h
	$(CC) $(CFLAGS) $(INCS) -c $<

dfs: dfs.o drive.o mbr.o bloc.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
	
adhoc.o: adhoc.c drive.h mbr.h bloc.h
	$(CC) $(CFLAGS) $(INCS) -c $<

adhoc: adhoc.o drive.o mbr.o bloc.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

compile: $(ALL)

clean:
	rm -f *.o *.s $(ALL)