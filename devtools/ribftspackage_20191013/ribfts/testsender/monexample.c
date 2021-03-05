/* babirl : devtool/monan.c
 * last modified : 07/01/18 14:37:31 
 * 
 * Hidetada Baba (RIKEN)
 * baba@ribf.riken.jp
 *
 * Monitor babian shared memory
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>


/* babirl */
#include "ridf.h"

#define BABIAN_SHM_BUFF_SIZE 0x80000
#define ANSHMKEY    561000   ///< Shared memory key for babian
#define ANSEMKEY    561001   ///< Semaphore key for babian shm

union semun {
  int val;                        /* value for SETVAL */
  struct semid_ds *buf;           /* buffer for IPC_STAT & IPC_SET */
  unsigned short *array;          /* array for GETALL & SETALL */
  struct seminfo *__buf;          /* buffer for IPC_INFO */
};



int main(int argc, char *argv[]){
  RIDFHD hd;
  RIDFRHD rhd;
  struct sembuf semb;
  union semun semunion;
  char *shmp, *fshmp;
  unsigned int blocknum, tblocknum;
  int shmid, semid, i;
  unsigned short sval;
  int shmsize, sid;

  sid = 0; // id for shared memory of udp client
  // sid 6 is special (8MB buffer) by babianpull
  // usually, we use sid = 0 

  shmsize = BABIAN_SHM_BUFF_SIZE;

  /* Shared Memory */
  if(!(shmid = initshm(ANSHMKEY+sid*2,
		       shmsize + sizeof(int),
		       &shmp))){
    printf("Can't create shared memory\n");
    exit(0);
  }
  
  /* Semaphore */
  if(!(semid = initsem(ANSEMKEY+sid*2, &semunion))){
    printf("Can't create semaphore\n");
    exit(0);
  }
  fshmp = shmp + shmsize;
  
  while(1){
    sem_p(semid, &semb);     // Lock shared memory
    memcpy((char *)&tblocknum, fshmp, sizeof(blocknum));
    // check the block number in the shared memory
    // if the block number is different from previous one
    // it means that there is new data 
    if(blocknum != tblocknum){
      blocknum = tblocknum;
      memcpy((char *)&hd, shmp, sizeof(hd));
      rhd = ridf_dechd(hd);
      printf("ly=%d, cid=%d, size=%d, efn=%d\n",
	     rhd.layer, rhd.classid, rhd.blksize, rhd.efn);         
      for(i=0;i<64;i+=2){
	if(i%16 == 0){
	  printf("\n");
	}
	memcpy((char *)&sval, shmp+i, sizeof(sval));
	printf("%04x ", sval);
      }
      printf("\n");
    }
    sem_v(semid, &semb);     // Unlock shared memory

    sleep(1);
  }

  return 0;
}
