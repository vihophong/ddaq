ALL : testsender

CFLAGS =   -DUNIX -DLINUX -DLINUX64 -DPOSIX -I/home/aida/ribfts/DataXferLib/V4_TCP
LOADLIBES=   -L/usr/ucblib -lpthread -L/home/aida/ribfts/MIDAS/Linux/lib64 -lxfer

testsender : testsender.o bi-tcp.o ridf.o bb-decode-V7XX.o

clean : 
	rm -f *.o
