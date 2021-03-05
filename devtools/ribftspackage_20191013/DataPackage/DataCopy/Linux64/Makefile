
.KEEP_STATE:

EG = /eurogam

OS=Linux64

COMPILE.C=	$(CC)  $(CFLAGS) $(CPPFLAGS) -c $(INCLUDES)
LINK.cc=        $(CC) 

CC = gcc
CFLAGS =   -DUNIX -DLINUX -DLINUX64 -DPOSIX

INCLUDES=  -I.  -I.. -I../../DataSpyLib
LDLIBS =   -lrt  -L/MIDAS/Linux/lib64 -ldataspy

%.o:	../%.c
	$(COMPILE.C) $<

TARGET = DataCopy

OBJECTS=   datacopy.o

$(TARGET): $(OBJECTS)
	$(LINK.cc) -o $@ $(OBJECTS) $(LDLIBS)
	@echo made $@

all: $(TARGET)

install:
	cp $(TARGET) $(EG)/Linux/bin64
	cp $(TARGET) /MIDAS/Linux/bin64

clean:
	rm -f *.o *%

