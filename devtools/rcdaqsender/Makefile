PACKAGE = myanalysis

DECODER_PACKAGE = pdecoder

ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --glibs)

CXXFLAGS = -O -DUSEPTHREAD -DSIXTYFOUR_BIT -DTSOVERLAP -DUNIX -DLINUX -DLINUX64 -DPOSIX -I/home/daq/brIKEN2019/usersanalysis/ribfts/DataStorage/ -I.  $(ROOTFLAGS) -I$(ONLINE_MAIN)/include -I$(OFFLINE_MAIN)/include 
RCFLAGS = -I.  -I$(ONLINE_MAIN)/include -I$(OFFLINE_MAIN)/include
AFLAGS = -I. -I/usr/lib
XFERFLAGS = -I/home/daq/brIKEN2019/usersanalysis/ribfts/DataXferLib/V4_TCP

LDFLAGS = -Wl,--no-as-needed  -L$(ONLINE_MAIN)/lib  -L$(OFFLINE_MAIN)/lib -lpmonitor -lEvent -lNoRootEvent -lmessage $(ROOTLIBS) -fPIC 

ALDFLAGS = -Wl,--no-as-needed $(ROOTLIBS) -fPIC


HDRFILES = $(DECODER_PACKAGE).h
LINKFILE = $(DECODER_PACKAGE)LinkDef.h

APKG = DataStruct
AHDRFILES = libDataStruct.h
ALINKFILE = libDataStructLinkDef.h
XFER = Xfer
XFERHDRFILES = transfer.h
XFERLINKFILE = transferLinkDef.h

ADDITIONAL_SOURCES = libDataStruct.h dpp.cpp dpp.h transfer.h transfer.c
ADDITIONAL_LIBS = -L$(PWD) -$(APKG) -L/usr/ucblib -lpthread

SO = lib$(PACKAGE).so

DATASO = lib$(APKG).so


$(SO) : $(DECODER_PACKAGE).cc $(PACKAGE).cc $(DECODER_PACKAGE)_dict.C $(ADDITIONAL_SOURCES) $(LINKFILE) $(APKG)_dict.C $(XFER)_dict.C $(DATASO)
	$(CXX) $(CXXFLAGS) -o $@ -shared  $<  $(ADDITIONAL_SOURCES) $(DECODER_PACKAGE)_dict.C $(APKG)_dict.C $(XFER)_dict.C $(LDFLAGS) $(ADDITIONAL_LIBS)

$(DATASO) : $(AHDRFILES) $(APKG)_dict.C
	$(CXX) -I. $(ROOTFLAGS) -o $@ -shared $^ $(ALDFLAGS)

$(DECODER_PACKAGE)_dict.C : $(HDRFILES) $(LINKFILE)
	rootcint -f $@  -c $(RCFLAGS) $^

$(APKG)_dict.C: $(AHDRFILES) $(ALINKFILE)
	rootcint -f $@  -c $(AFLAGS) $^

$(XFER)_dict.C: $(XFERHDRFILES) $(XFERLINKFILE)
	rootcint -f $@  -c $(XFERFLAGS) $^
 
.PHONY: clean

clean: 
	rm -f $(SO) $(DATASO) $(DECODER_PACKAGE)_dict.C $(DECODER_PACKAGE)_dict.h $(APKG)_dict.C $(APKG)_dict.h $(XFER)_dict.C

