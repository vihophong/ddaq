
.KEEP_STATE:

EG = /eurogam

OS=Linux64

COMPILE.C=	$(CC)  $(CFLAGS) $(CPPFLAGS) -c $(INCLUDES)
LINK.cc=        $(CC) 

CC = gcc

INCLUDES=  -I.  -I.. -I../../DataSpyLib
LDLIBS =   -lrt  -L/MIDAS/Linux/lib64 -ldataspy

%.o:	../%.c
	$(COMPILE.C) $<

TARGET = DataPeek

OBJECTS=   datapeek.o

$(TARGET): $(OBJECTS)
	$(LINK.cc) -o $@ $(OBJECTS) $(LDLIBS)
	@echo made $@

all: $(TARGET)

install:
	cp $(TARGET) $(EG)/Linux/bin64
	cp $(TARGET) /MIDAS/Linux/bin64

clean:
	rm -f *.o *%
