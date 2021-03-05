ROOT= $(ENV_PREFIX)


CC        =  $(CROSS_COMPILE)gcc
CFLAGS    = -O -DLINUX -DUNIX -DPOSIX -DTSOVERLAP

COMPILE.c= $(CC) $(CFLAGS) $(CPPFLAGS)
LINK.c=    $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

INCLUDES=	  -I.  -I.. -I../.. -I../../xfer/include
LDLIBS=   -L/usr/ucblib -lpthread

all: DataGen

main.o:
	$(COMPILE.c) -c ../main.c $(OUTPUT_OPTION) $(INCLUDES) $<

transfer.o:
	$(COMPILE.c) -c ../transfer.c $(OUTPUT_OPTION) $(INCLUDES) $<

DataGen: main.o transfer.o
	$(LINK.c) -o $@ main.o transfer.o  $(LDLIBS)

clean:
	rm -f  *.o  *~ *%

