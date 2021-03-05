
.KEEP_STATE:

EG = /eurogam

OS=Linux64

COMPILE.C=	$(CC)  $(CFLAGS) $(CPPFLAGS) -c $(INCLUDES)
LINK.cc=        $(CC) 

CC = gcc
CFLAGS =   -DUNIX -DLINUX -DLINUX64 -DPOSIX

INCLUDES=  -I.  -I.. -I../../DataXferLib/V4_TCP -I../../DataSpyLib
LDLIBS=   -L/usr/ucblib -lrt -lpthread -L/MIDAS/Linux/lib64 -lxfer -ldataspy

%.o:	../%.c
	$(COMPILE.C) $<

TARGET = DataRelay

OBJECTS=   datarelay.o

$(TARGET): $(OBJECTS)
	$(LINK.cc) -o $@ $(OBJECTS) $(LDLIBS)
	@echo made $@

all: $(TARGET)

install:
	cp $(TARGET) $(EG)/Linux/bin64
	cp $(TARGET) /MIDAS/Linux/bin64

clean:
	rm -f *.o *%


