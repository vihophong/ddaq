.KEEP_STATE:

EG = /eurogam

OS=Linux64

COMPILE.C=	$(CC)  $(CFLAGS) $(CPPFLAGS) -c $(INCLUDES)
LINK.cc=        $(CC) 

CC = g++
CFLAGS =   -DUNIX -DLINUX -DLINUX64 -DPOSIX -DNOLOGGING -DSINK -DMASTER

CCHEADERS = /usr/include

INCLUDES=	-I$(CCHEADERS) -I../../DataStorage -I../../DataXferLib/V4_TCP/
LDLIBS =  -lrt


%.o:	../%.cpp
	$(COMPILE.C) $<

%.o:	../../DataStorage/%.cpp
	$(COMPILE.C) $<

%:	../%.cpp
	$(LINK.cc) -o $@ $< $(LDLIBS)

TARGET = DataSink

OBJECTS=   datasink.o link_server.o alloc_data_area.o  data_transfer.o


$(TARGET): $(OBJECTS)
	$(LINK.cc) -o $@ $(OBJECTS) $(LDLIBS)
	@echo made $@

all: $(TARGET)

install:
	cp $(TARGET) $(EG)/Linux/bin64
	cp $(TARGET) /MIDAS/Linux/bin64

clean:
	rm -f *.o *%

