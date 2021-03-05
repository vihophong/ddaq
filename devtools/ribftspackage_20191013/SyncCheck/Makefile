UNAME := $(shell uname -s)
#rootcint -f RootDisplayDict.cxx -c RootDisplay.h  RootDisplayLinkDef.h 
CC = g++ /Linux/lib64

#CFLAGS= -c -g -Wall `root-config --cflags`
# LDFLAGS=`root-config --glibs`

INC  = -I../inc  -I. -I$(ROOTSYS)/include -I.. -I/home/daq/ribftspackage_20191013/DataPackage/DataSpyLib
FLAGS = -c $(INC)  -std=c++0x 
LIBFLAG = $(LIB)   -std=c++0x  -lrt  -L/home/daq/ribftspackage_20191013/DataPackage/MIDAS/Linux/lib64 -ldataspy

SyncCheck:  SyncCheck.cpp


	@echo Makefile to compile in Linux
	@echo Compiling RootDisplay flags: $(FLAGS)

	@$(CXX) SyncCheck.cpp -o SyncCheck.o $(FLAGS)$(LDFLAGS)
	@echo Linking RootDisplay flags: $(LIBFLAG)$(CFLAGS)
	@$(CXX) SyncCheck.o SyncCheckDict.cxx -o SyncCheck $(LIBFLAG) `root-config --cflags --glibs`
	@echo Done!

SyncCheckTAS:  SyncCheckTAS.cpp


	@echo Makefile to compile in Linux
	@echo Compiling RootDisplay flags: $(FLAGS)

	@$(CXX) SyncCheckTAS.cpp -o SyncCheckTAS.o $(FLAGS)$(LDFLAGS)
	@echo Linking RootDisplay flags: $(LIBFLAG)$(CFLAGS)
	@$(CXX) SyncCheckTAS.o SyncCheckDict.cxx -o SyncCheckTAS $(LIBFLAG) `root-config --cflags --glibs`
	@echo Done!





