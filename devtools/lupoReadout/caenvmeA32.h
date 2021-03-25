#ifndef CAENVMEA32_H
#define CAENVMEA32_H
#define LINUX 1
int init_nbbqvio(int* ret, short Link, short Device);
unsigned short vme_read16(int BHandle,unsigned int addr);
unsigned int vme_read32(int BHandle,unsigned int addr);
int vme_readBlock32(int BHandle,unsigned int addr,unsigned int* Data,int size);
int vme_write16(int BHandle,unsigned int addr,unsigned short sval);
int vme_write16a16(int BHandle,unsigned int addr,unsigned short sval);
int vme_write32(int BHandle,unsigned int addr,unsigned int lval);
int vme_amsr(unsigned int lval);
int vread32(int BHandle,unsigned int addr, int *val);
short vread16(int BHandle,unsigned int addr, short *val);
void vwrite32(int BHandle,unsigned int addr, int *val);
void vwrite16(int BHandle,unsigned int addr, short *val);
void vwrite16a16(int BHandle,unsigned int addr, short *val);
void release_nbbqvio(int BHandle);

#endif // CAENVMEA32_H
