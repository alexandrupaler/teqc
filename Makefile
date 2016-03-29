CC = g++
CS = gcc
INCLUDES = ./headers
CFLAGS = -I$(INCLUDES) -lm -g #-m32 #-O3
CXXFLAGS =$(CFLAGS) 

export CPATH=$CPATH:$(INCLUDES)

OUTPUT = convertft
FILES = $(FILESDB) circuitmatrix.o numberandcoordinate.o geometry.o circuitgeometry.o \
	fileformats/psfilewriter.o fileformats/qcircfilewriter.o \
	fileformats/geomfilewriter.o fileformats/chpfilewriter.o \
	fileformats/iofilewriter.o fileformats/boundingboxfilewriter.o \
	fileformats/infilereader.o cnotcounter.o oldconvertft.o 

OUTPUTDB = dbread
FILESDB = decomposition.o databasereader.o

OUTPUTB = boxworld
FILESB = fileformats/boxcoordfilewriter.o fileformats/iofilereader.o fileformats/iofilewriter.o schedulerspace.o boxworld2.o computeadditional.o numberandcoordinate.o boxworld2_main.o

OUTPUTF = faildistillations
FILESF =  fileformats/toconnectfilewriter.o fileformats/iofilereader.o faildistillations.o numberandcoordinate.o faildistillations_main.o

OUTPUTP = processraw
FILESP = $(FILESDB) cnotcounter.o  processraw.o fileformats/rawfilereader.o fileformats/infilewriter.o processraw_main.o

OUTPUTA = additional
FILESA = additional.o computeadditional.o

OUTPUTC = chp
FILESC = chp.o

OUTPUTCONN = connectpins
FILESCONN = fileformats/geomfilewriter.o fileformats/iofilereader.o faildistillations.o numberandcoordinate.o geometry.o connectpins.o connectpinsmain.o


all:: process convert boxworld additional faild connectpins chp

test::
	make -C geomtest

additional:: $(FILESA)
	$(CC) $(CFLAGS) $(FILESA) -o $(OUTPUTA)
	
dbread:: $(FILESDB)
	$(CC) $(CFLAGS) $(FILESDB) -o $(OUTPUTDB)

boxworld:: $(FILESB)
	$(CC) $(CFLAGS) $(FILESB) -o $(OUTPUTB)
	
faild:: $(FILESF)
	$(CC) $(CFLAGS) $(FILESF) -o $(OUTPUTF)

convert:: $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o $(OUTPUT)
	
process:: $(FILESP)
	$(CC)  $(CFLAGS) $(FILESP) -o $(OUTPUTP)
	
chp:: $(FILESC)
	$(CS)  $(FILESC) -o $(OUTPUTC)	
	
connectpins:: $(FILESCONN)
	$(CC) $(CFLAGS) $(FILESCONN) -o $(OUTPUTCONN)	

clean::
	rm -f $(OUTPUT) $(FILES)
	rm -f $(OUTPUTP) $(FILESP)
	rm -f $(OUTPUTA) $(FILESA)
	rm -f $(OUTPUTC) $(FILESC)
	rm -f $(OUTPUTB) $(FILESB)
	rm -f $(OUTPUTDB) $(FILESDB)
	rm -f $(OUTPUTF) $(FILESF)
	rm -f $(OUTPUTCONN) $(FILESCONN)

ps::
	cat templateh.ps test.ps templatec.ps > circ.ps
	epstopdf circ.ps
