#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include <graphics/gfx.h>

#include <stdio.h>

#include "ShadowMaskInterleaved.h"

ShadowMaskInterleaved::ShadowMaskInterleaved(struct BitMap* pImage)
  : m_pMask(NULL),
    m_MaskSizeBytes(0)
{
  struct BitMap *pMaskBitMap, *pTempBitMap;
  struct RastPort rp, temprp;
  UBYTE *pPixelArray, *pY, *pX, *pSrcByte, *pDstByte;
  ULONG x, y, numMaskCopies, bytesPerRow, arrayBytes, i, iSrcCol, iSrcRow, iDstRow;
  long transparentPen = 0;

  if(pImage == NULL)
  {
    throw "ShadowMaskInterleaved: No BitMap image given.";
  }

  m_Width = GetBitMapAttr(pImage, BMA_WIDTH);
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
  m_Height = GetBitMapAttr(pImage, BMA_HEIGHT);
  m_Depth = GetBitMapAttr(pImage, BMA_DEPTH);
  ULONG planeSize = RASSIZE(m_Width, m_Height);

  // Default: Source BitMap is planar, only one copy of the mask needed
  numMaskCopies = 1;

  // But check if source BitMap is interleaved
  if(GetBitMapAttr(pImage, BMA_FLAGS) & BMF_INTERLEAVED)
  {
    // Source BitMap is interleaved; must copy mask m_Depth times
    numMaskCopies = m_Depth;
  }

  m_MaskSizeBytes = planeSize * numMaskCopies;

  // Allocate memory for the final mask (m_Depth * m_Height because interleaved)
  m_pMask = (UBYTE*)AllocVec(m_MaskSizeBytes, MEMF_CHIP|MEMF_CLEAR);
  if(m_pMask == NULL)
  {
    throw "ShadowMaskInterleaved: Failed to allocate memory for frame mask.";
  }

  // Allocate temporary Bitmap to write the mask
  pMaskBitMap = AllocBitMap (m_Width, m_Height, 1, BMF_CLEAR, NULL);
  if(pMaskBitMap == NULL)
  {
    FreeRaster(m_pMask, m_Width, m_Depth * m_Height);
    throw "ShadowMaskInterleaved: Failed to allocate temporary mask BitMap.";
  }


  pTempBitMap = AllocBitMap (m_Width, m_Height, m_Depth, BMF_CLEAR, NULL);
  if(pTempBitMap == NULL)
  {
    FreeBitMap(pMaskBitMap);
    FreeRaster(m_pMask, m_Width, m_Depth * m_Height);
    throw "ShadowMaskInterleaved: Failed to allocate temporary BitMap.";
  }

  // See autodocs for Read/WritePixelArray8
  arrayBytes = ((((m_Width + 15) >> 4) << 4) * (m_Height + 1));
  bytesPerRow = arrayBytes / m_Height;

  // Allocate memory for PixelArray
  pPixelArray = (UBYTE*)AllocVec (arrayBytes, MEMF_PUBLIC);
  if(pPixelArray == NULL)
  {
    FreeBitMap(pTempBitMap);
    FreeBitMap(pMaskBitMap);
    FreeRaster(m_pMask, m_Width, m_Depth * m_Height);
    throw "ShadowMaskInterleaved: Failed to allocate memory for pixel array.";
  }

  InitRastPort (&rp);
  rp.BitMap = pImage;  // Point the RastPort to source BitMap to create
                        // the PixelArray from it

  // as defined in autodocs for Read/WritePixelArray8
  temprp = rp;
  temprp.Layer = NULL;
  temprp.BitMap = pTempBitMap;

  ReadPixelArray8 (&rp, 0, 0, m_Width - 1, m_Height - 1, pPixelArray, &temprp);

  pY = pPixelArray;
  for (y = 0; y < m_Height; y++)
  {
    pX = pY;

    for (x = 0; x < m_Width; x++)
    {
      // set transparent pixels to 0, others to 1
      if(*pX == transparentPen)
      {
        *pX = 0;
      }
      else
      {
        *pX = 1;
      }

      pX++;
    }

    pY += bytesPerRow;
  }

  // Set destination for WritePixelArray8 to mask BitMap
  rp.BitMap = pMaskBitMap;

  // Write the mask
	WritePixelArray8 (&rp, 0, 0, m_Width - 1, m_Height - 1, pPixelArray, &temprp);

  // Now use the 'real' BytesPerRow from the planar / non-interleaved
  // mask BitMap
  bytesPerRow = pMaskBitMap->BytesPerRow;

  iSrcRow = 0;
  iDstRow = 0;
  do
  {
    for(iSrcCol = 0; iSrcCol < bytesPerRow; iSrcCol++)
    {
      // Address the correct byte in planar source mask
      pSrcByte = pMaskBitMap->Planes[0] + ((iSrcRow * bytesPerRow) + iSrcCol);

      // Address the correct byte in interleaved destination mask
      pDstByte = m_pMask + ((iDstRow * bytesPerRow) + iSrcCol);

      // Perform the needed number of copies vertically in this row
      for(i = 0; i < numMaskCopies; i++)
      {
        *pDstByte = *pSrcByte;
        pDstByte += bytesPerRow;  // Address the byte below the current one
      }
    }

    iSrcRow++;
    iDstRow += numMaskCopies; // Address the row below the last copy destination row
  } while (iSrcRow < m_Height);

  FreeVec(pPixelArray);
  FreeBitMap(pTempBitMap);
  FreeBitMap(pMaskBitMap);
}


ShadowMaskInterleaved::~ShadowMaskInterleaved()
{
  delete[] m_pRowPixels;

  if(m_pMask != NULL)
  {
    FreeVec(m_pMask);
    m_pMask = NULL;
  }
}


void ShadowMaskInterleaved::Print()
{
  ULONG bytesPerRow = m_WordWidth * 2;
  for(ULONG i = 0; i < m_MaskSizeBytes; i++)
  {
    printBits(1, m_pMask + i);
    printf(" ");

    if((i + 1) % bytesPerRow == 0)
    {
      printf("\n");
    }
  }
}


// assumes little endian
void ShadowMaskInterleaved::printBits(size_t const size, void const* const ptr)
{
  unsigned char* p = (unsigned char*)ptr;
  unsigned char byte;
  int i, j;

  for (i = size - 1; i >= 0; i--)
  {
    for (j = 7; j >= 0; j--)
    {
      byte = (p[i] >> j) & 1;
      printf("%u", byte);
    }
  }
}