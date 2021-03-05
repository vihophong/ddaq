#include "typedefs.h"

/*
 * N. Kurz:
 * next follow the masks to extract crate, station, address and function
 * of a CAMAC cnaf
 */

#define CNAF__MASK_C 0x1e00000
#define CNAF__MASK_N 0xf800
#define CNAF__MASK_A 0x3c
#define CNAF__MASK_F 0x7c0
