#include <dos/dos.h>
#include <graphics/gfx.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>

#include <stdio.h>

#include "imagetools.h"

struct BitMap *LoadRawBitMap(const char *pPath,
                             int width,
                             int height,
                             short depth)
{
  if (pPath == NULL)
  {
    return NULL;
  }

  BPTR fileHandle = Open(pPath, MODE_OLDFILE);
  if (fileHandle == 0)
  {
    return NULL;
  }

  ULONG fl = BMF_CLEAR; //|BMF_INTERLEAVED;
  struct BitMap *pBitMap = AllocBitMap(width, height, depth, fl, NULL);

  if (pBitMap == NULL)
  {
    // Couldn't get BitMap
    Close(fileHandle);
    return NULL;
  }

  // Check if we really got an interleaved bitmap
  fl = GetBitMapAttr(pBitMap, BMA_FLAGS);
  if (fl & BMF_INTERLEAVED)
  {
    // We got an interleaved BitMap, so reading all data at once
    int planeSize = RASSIZE(width, height) * depth;
    if (Read(fileHandle, pBitMap->Planes[0], planeSize) != planeSize)
    {
      // Error while reading
      FreeBitMap(pBitMap);
      Close(fileHandle);
      return NULL;
    }
  }
  else
  {
    // No interleaved BitMap, so reading all plane ptrs individually
    int planeSize = RASSIZE(width, height);
    for (int i = 0; i < depth; i++)
    {
      if (Read(fileHandle, pBitMap->Planes[i], planeSize) != planeSize)
      {
        // Error while reading
        FreeBitMap(pBitMap);
        Close(fileHandle);
        return NULL;
      }
    }
  }

  Close(fileHandle);
  return pBitMap;
}

void FreeRawBitMap(struct BitMap *pBitMap)
{
  if (pBitMap == NULL)
  {
    return;
  }

  FreeBitMap(pBitMap);
}

struct BitMap *CreateBitMapMask(struct BitMap *pSrcBm,
                                int width, int height)
{
  if (pSrcBm == NULL)
  {
    return NULL;
  }

  struct BitMap *pMaskBm = AllocBitMap(width, height, 1, BMF_CLEAR, NULL);
  return pMaskBm;

  int numBytes = pSrcBm->BytesPerRow * pSrcBm->Rows;
  for (int i = 0; i < numBytes; i++)
  {
    UBYTE maskByte = 0;

    for (int j = 0; j < pSrcBm->Depth; j++)
    {
      UBYTE *plane = pSrcBm->Planes[j];
      maskByte |= plane[i];
    }

    //pMask[i] = maskByte;
  }

  return pMaskBm;
}

void FreeBitMapMask(struct BitMap *pMaskBm)
{
  if (pMaskBm == NULL)
  {
    return;
  }

  FreeBitMap(pMaskBm);
}

WORD *LoadRawImageData(APTR pMemoryPool,
                       const char *pPath,
                       int width,
                       int height,
                       short depth)
{
  if (pPath == NULL)
  {
    return NULL;
  }

  BPTR fileHandle = Open(pPath, MODE_OLDFILE);
  if (fileHandle == 0)
  {
    return NULL;
  }

  // Determine needed memory size for image data
  LONG wordWidth = ((width + 15) & -16) >> 4;
  LONG bufSizeBytes = wordWidth * 2 * height * depth;

  WORD *pImgData = (WORD *)AllocPooled(pMemoryPool, bufSizeBytes);

  // Read the file data into target chip memory buffer
  if (Read(fileHandle, pImgData, bufSizeBytes) != bufSizeBytes)
  {
    // Error while reading
    Close(fileHandle);
    FreePooled(pMemoryPool, pImgData, bufSizeBytes);
    return NULL;
  }

  Close(fileHandle);
  return pImgData;
}
