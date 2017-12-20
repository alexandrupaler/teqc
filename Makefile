OBJDIR = obj
BINDIR = bin
INCLUDES = headers
CXXFLAGS =$(CFLAGS) 

CC = g++
CFLAGS = -Wall -I$(INCLUDES) -lm -g

export CPATH=$CPATH:$(INCLUDES)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	@mkdir -p $@
	
$(BINDIR):
	@mkdir -p $@

OUTPUT = $(BINDIR)/convertft
FILES = $(FILESDB) circuitmatrix.cpp numberandcoordinate.cpp geometry.cpp circuitgeometry.cpp \
	$(wildcard fileformats/*.cpp) \
	$(wildcard scheduling/*.cpp) \
	$(wildcard astar/*.cpp) \
	$(wildcard pins/*.cpp) \
	$(wildcard recycling/*.cpp) \
	oldconvertft.cpp \
	gatenumbers.cpp cnotcounter.cpp cuccaro.cpp \
	circconvert.cpp\
	computeadditional.cpp plumbingpieces.cpp\
	boxworld2.cpp faildistillations.cpp \
	connectpins.cpp heuristicparameters.cpp \
	convertcoordinate.cpp\
	connectionspool.cpp connectionmanager.cpp
	
#	fileformats/psfilewriter.cpp fileformats/qcircfilewriter.cpp \
	fileformats/geomfilewriter.cpp fileformats/adamfilewriter.cpp fileformats/chpfilewriter.cpp \
	fileformats/iofilewriter.cpp fileformats/boundingboxfilewriter.cpp \
	fileformats/infilereader.cpp 
	
#fileformats/toconnectfilewriter.cpp fileformats/boxcoordfilewriter.cpp \
	fileformats/iofilereader.cpp \
	
	
#	causaltogeom.cpp \
	
OFILES = $(patsubst %.cpp, $(OBJDIR)/%.o, $(FILES))
	
OUTPUTDB = $(BINDIR)/dbread
FILESDB = decomposition.cpp databasereader.cpp
OFILESDB = $(patsubst %.cpp, $(OBJDIR)/%.o, $(FILESDB))

OUTPUTB = $(BINDIR)/boxworld
FILESB = fileformats/toconnectfilewriter.cpp fileformats/boxcoordfilewriter.cpp \
		fileformats/iofilereader.cpp fileformats/iofilewriter.cpp schedulerspace.cpp \
		boxworld2.cpp computeadditional.cpp numberandcoordinate.cpp faildistillations.cpp \
		fileformats/geomfilewriter.cpp connectpins.cpp geometry.cpp\
		boxworld2_main.cpp astaralg.cpp astarpoint.cpp
OFILESB = $(patsubst %.cpp, $(OBJDIR)/%.o, $(FILESB))

OUTPUTF = $(BINDIR)/faildistillations
FILESF =  fileformats/toconnectfilewriter.cpp fileformats/iofilereader.cpp \
		faildistillations.cpp numberandcoordinate.cpp faildistillations_main.cpp
OFILESF = $(patsubst %.cpp, $(OBJDIR)/%.o, $(FILESF))

OUTPUTP = $(BINDIR)/processraw
FILESP = $(FILESDB) cnotcounter.cpp  processraw.cpp fileformats/rawfilereader.cpp \
		fileformats/infilewriter.cpp processraw_main.cpp
OFILESP = $(patsubst %.cpp, $(OBJDIR)/%.o, $(FILESP))

OUTPUTA = $(BINDIR)/additional
FILESA = additional.cpp computeadditional.cpp
OFILESA = $(patsubst %.cpp, $(OBJDIR)/%.o, $(FILESA))

OUTPUTC = $(BINDIR)/chp
FILESC = chp.c
OFILESC = $(patsubst %.cpp, $(OBJDIR)/%.o, $(FILESC))

OUTPUTCONN = $(BINDIR)/connectpins
FILESCONN = fileformats/geomfilewriter.cpp fileformats/iofilereader.cpp \
	faildistillations.cpp numberandcoordinate.cpp geometry.cpp \
	connectpins.cpp connectpinsmain.cpp astaralg.cpp astarpoint.cpp
OFILESCONN = $(patsubst %.cpp, $(OBJDIR)/%.o, $(FILESCONN))

#all:: process convert boxworld additional faild connectpins chp

all:: convert

test::
	make -C geomtest

additional:: $(OFILESA) $(BINDIR)
	$(CC) $(CFLAGS) $(OFILESA) -o $(OUTPUTA)
	
dbread:: $(OFILESDB) $(BINDIR)
	$(CC) $(CFLAGS) $(OFILESDB) -o $(OUTPUTDB)

boxworld:: $(OFILESB) $(BINDIR)
	$(CC) $(CFLAGS) $(OFILESB) -o $(OUTPUTB)
	
faild:: $(OFILESF) $(BINDIR)
	$(CC) $(CFLAGS) $(OFILESF) -o $(OUTPUTF)

convert:: $(OFILES) $(BINDIR)
	$(CC) $(CFLAGS) $(OFILES) -o $(OUTPUT)
	
process:: $(OFILESP) $(BINDIR)
	$(CC)  $(CFLAGS) $(OFILESP) -o $(OUTPUTP)
	
chp:: $(OFILESC) $(BINDIR)
	$(CS)  $(OFILESC) -o $(OUTPUTC)	
	
connectpins:: $(OFILESCONN) $(BINDIR)
	$(CC) $(CFLAGS) $(OFILESCONN) -o $(OUTPUTCONN)	

#clean::
#	rm -f $(OUTPUT) $(FILES)
#	rm -f $(OUTPUTP) $(FILESP)
#	rm -f $(OUTPUTA) $(FILESA)
#	rm -f $(OUTPUTC) $(FILESC)
#	rm -f $(OUTPUTB) $(FILESB)
#	rm -f $(OUTPUTDB) $(FILESDB)
#	rm -f $(OUTPUTF) $(FILESF)
#	rm -f $(OUTPUTCONN) $(FILESCONN)

ps::
	cat templateh.ps test.ps templatec.ps > circ.ps
	epstopdf circ.ps
