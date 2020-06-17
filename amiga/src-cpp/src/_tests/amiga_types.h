#ifndef AMIGA_TYPES_H
#define AMIGA_TYPES_H

#include <stddef.h>

typedef unsigned long   ULONG;      /* unsigned 32-bit quantity */
typedef unsigned short  UWORD;      /* unsigned 16-bit quantity */
typedef unsigned char   UBYTE;      /* unsigned 8-bit quantity */
typedef UBYTE *PLANEPTR;

struct BitMap
{
    UWORD   BytesPerRow;
    UWORD   Rows;
    UBYTE   Flags;
    UBYTE   Depth;
    UWORD   pad;
    PLANEPTR Planes[8];
};

void* AllocVec( ULONG byteSize, ULONG requirements );
void FreeVec( void* memoryBlock );



#define MEMF_ANY    (0L)	/* Any type of memory will do */
#define MEMF_CLEAR   (1L<<16)	/* AllocMem: NULL out area before return */


#endif
