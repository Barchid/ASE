ROOTDIR=/home/enseign/ASE

ALL = dmps adhoc frmt
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

dmps.o: dmps.c drive.h hw.h
	$(CC) $(CFLAGS) $(INCS) -c $<

dmps: dmps.o drive.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

adhoc.o: adhoc.c drive.h hw.h
	$(CC) $(CFLAGS) $(INCS) -c $<

adhoc: adhoc.o drive.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

frmt.o: frmt.c drive.h hw.h
	$(CC) $(CFLAGS) $(INCS) -c $<

frmt: frmt.o drive.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

execDmps: dmps
	./dmps

execAdhoc: adhoc
	./adhoc

execFrmt: frmt
	./frmt

compile: $(ALL)

clean:
	rm -f *.o *.s dmps adhoc frmt