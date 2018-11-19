ROOTDIR=/home/enseign/ASE

ALL = mknfs dfs mkvol dvol adhoc if_cfile if_dfile if_nfile if_pfile
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

inode.o: inode.c inode.h drive.h mbr.h bloc.h hw.h
	$(CC) $(CFLAGS) $(INCS) -c $<

ifile.o: ifile.c inode.h tools.h ifile.h 
	$(CC) $(CFLAGS) $(INCS) -c $<
	
mount.o: mount.c hw.h tools.h 
	$(CC) $(CFLAGS) $(INCS) -c $<
	
tools.o: tools.c tools.h
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

if_cfile.o: if_cfile.c ifile.h mount.h
	$(CC) $(CFLAGS) $(INCS) -c $<

if_cfile: if_cfile.o tools.o ifile.o mount.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
	
if_dfile.o: if_dfile.c ifile.h mount.h
	$(CC) $(CFLAGS) $(INCS) -c $<

if_dfile: if_dfile.o tools.o ifile.o mount.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
	
if_nfile.o: if_nfile.c ifile.h mount.h
	$(CC) $(CFLAGS) $(INCS) -c $<

if_nfile: if_nfile.o tools.o ifile.o mount.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
	
if_pfile.o: if_pfile.c ifile.h mount.h
	$(CC) $(CFLAGS) $(INCS) -c $<

if_pfile: if_pfile.o tools.o ifile.o mount.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

compile: $(ALL)

clean:
	rm -f *.o *.s $(ALL)
	
	