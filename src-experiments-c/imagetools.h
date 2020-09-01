#ifndef BITMAP_FUNCTIONS_H
#define BITMAP_FUNCTIONS_H

#include <exec/types.h>
#include <graphics/gfx.h>

/**
 * Loading an image raw file of given path and dimensions into 
 * a BitMap.
 * 
 * After usage the BitMap has to be freed with FreeRawBitMap().
 */
struct BitMap* LoadRawBitMap(const char* pPath,
                             int width,
                             int height,
                             short depth);

/**
 * Frees the BitMap created by LoadRawBitMap().
 */
void FreeRawBitMap(struct BitMap* pBitMap);


/**
 * Creates a BitMap with depth=1 masking out all bits that are 0 in all
 *  planes of the source BitMap.
 * 
 * After usage the BitMap has to be freed with FreeBitMapMask().
 */
struct BitMap* CreateBitMapMask(struct BitMap* pSrcBm,
                                int width, int height);
/**
 * Frees the BitMap created by CreateBitMapMask().
 */
void FreeBitMapMask(struct BitMap* pMaskBm);


/**
 * Loading an image raw file of given path and dimensions into 
 * a displayable meory region. The data then can be used as 
 * Bob ImageData within the GELs system.
 * 
 * TODO Check: can it be used as VSprite image data if saved in such
 * a format?
 * 
 * NOTE: When finished no freeing of the retrieved ImageData is 
 *       necessary as it is done automatically as soon as the 
 *       memory pool is deleted.
 */
WORD* LoadRawImageData(APTR pMemoryPool,
                       const char* pPath,
                       int width,
                       int height,
                       short depth);

#endif

