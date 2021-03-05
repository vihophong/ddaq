#ifndef LYNX

#include "typedefs.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/errno.h>

#define SM_READ SHM_R
#define SM_WRITE SHM_W
#define SM_DETACH (SHM_R<<2)
#define SM_NCREAT (SHM_R<<3)

INTS4 f_get_shm_id(CHARS *seg_name);
CHARS * f_get_seg_name(INTS4 shm_id);
CHARS * f_add_shm_table(CHARS *seg_name, INTS4 shm_id);
void f_rem_shm_table(CHARS *seg_name);
CHARS *smem_get(CHARS *name, INTS4 size, INTS4 perm);
CHARS *smem_create(CHARS *name, CHARS *physaddr, INTS4 size, INTS4 perm);
INTS4 smem_remove(CHARS *name);
#endif

