ALL : ribftssender

CFLAGS =   -DUNIX -DLINUX -DLINUX64 -DPOSIX -I/home/daq/ribftspackage_20191013/ribfts/DataXferLib/V4_TCP
LOADLIBES=   -L/usr/ucblib -lpthread -L/home/daq/ribftspackage_20191013/ribfts/MIDAS/Linux/lib64 -lxfer

ribftssender_mar17ca : ribftssender_mar17ca.o bi-tcp.o ridf.o 

ribftssender_nov17 : ribftssender_nov17.o bi-tcp.o ridf.o 
ribftssender : ribftssender.o bi-tcp.o ridf.o

clean : 
	rm -f *.o
