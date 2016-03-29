CC = g++
CFLAGS = -g -lm -lgtest -lpthread

export CPATH=$CPATH:../headers:..

MAINFILE = maintest.o
MAINOUT = maintest

CAFILES = computeadditional_test.o ../computeadditional.o
CPFILES = connectpins_test.o ../connectpins.o ../fileformats/geomfilewriter.o ../fileformats/iofilereader.o ../faildistillations.o ../numberandcoordinate.o ../geometry.o
BWFILES = ../boxworld2.o boxworld_test.o
SSFILES = ../schedulerspace.o ../numberandcoordinate.o schedulerspace_test.o
CMFILES = ../cnotcounter.o ../circuitmatrix.o circuitmatrix_test.o

all:: computeadditional connectpins faildistillations boxworld scheduler 

circuitm:: $(CMFILES) $(MAINFILE)
	$(CC) -o $(MAINOUT) $(CMFILES) $(MAINFILE) $(CFLAGS)

scheduler:: $(SSFILES) $(MAINFILE)
	$(CC) -o $(MAINOUT) $(SSFILES) $(MAINFILE) $(CFLAGS) 

boxworld:: $(BWFILES) $(MAINFILE)
	$(CC) -o $(MAINOUT) $(BWFILES) $(MAINFILE) $(CFLAGS) 

faildistillations:: $(FDFILES) $(MAINFILE)
	$(CC) -o $(MAINOUT) $(FDFILES) $(MAINFILE) $(CFLAGS)   

computeadditional:: $(CAFILES) $(MAINFILE)
	$(CC) -o $(MAINOUT) $(CAFILES) $(MAINFILE) $(CFLAGS)
	
connectpins:: $(CPFILES) $(MAINFILE)
	$(CC) -o $(MAINOUT) $(CPFILES) $(MAINFILE) $(CFLAGS)
