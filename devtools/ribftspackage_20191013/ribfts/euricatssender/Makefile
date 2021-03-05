ALL : euricatssender

CFLAGS =   -DUNIX -DLINUX -DLINUX64 -DPOSIX -I/home/aida/ribfts/DataXferLib/V4_TCP -I../eventapiV50
LOADLIBES=   -L/usr/ucblib -lpthread -L/home/aida/ribfts/MIDAS/Linux/lib64 -lxfer -lrt -lm

euricatssender : euricatssender.o bi-tcp.o ridf.o ../eventapiV50/libgsievt.a

clean : 
	rm -f *.o
