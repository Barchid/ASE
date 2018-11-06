ROOTDIR=/home/enseign/ASE

ALL = mkvol dvol vm
CC	= gcc
CFLAGS	= -m32 -Wall -pedantic 
CFLAGS  += -g
INCS    = -I$(ROOTDIR)/include
LIBS    = -L$(ROOTDIR)/lib -lhardware

###------------------------------------------------------------
### Main rules 
###------------------------------------------------------------

drive.o : drive.c drive.h hw.h
	$(CC) $(CFLAGS) $(INCS) -c $<

mbr.o: mbr.c mbr.h drive.h hw.h
	$(CC) $(CFLAGS) $(INCS) -c $<

mkvol.o: mkvol.c drive.h mbr.h
	$(CC) $(CFLAGS) $(INCS) -c $<

mkvol: mkvol.o drive.o mbr.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

dvol.o: dvol.c drive.h mbr.h
	$(CC) $(CFLAGS) $(INCS) -c $<

dvol: dvol.o drive.o mbr.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

vm.o: vm.c drive.h mbr.h
	$(CC) $(CFLAGS) $(INCS) -c $<

vm: vm.o drive.o mbr.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

compile: $(ALL)

clean:
	rm -f *.o *.s mkvol dvol vm