#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include <graphics/gfx.h>

#include <stdio.h>

#include "ShadowMaskInterleaved.h"

ShadowMaskInterleaved::ShadowMaskInterleaved(struct BitMap* pImage)
  : m_pMask(NULL),
    m_MaskSizeBytes(0)
{
  UBYTE *pMaskPlanar, *pSrcByte, *pDstByte, *pPlane;
  UBYTE maskedByte;
  ULONG numMaskCopies, bytesPerRow, i, j, iSrcCol, iSrcRow, iDstRow;

  if(pImage == NULL)
  {
    throw "ShadowMaskInterleaved: No BitMap image given.";
  }

  m_Width = GetBitMapAttr(pImage, BMA_WIDTH);
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
  m_Height = GetBitMapAttr(pImage, BMA_HEIGHT);
  m_Depth = GetBitMapAttr(pImage, BMA_DEPTH);
  ULONG planeSizeBytes = RASSIZE(m_Width, m_Height);

  // Default: Source BitMap is planar, only one copy of the mask needed
  numMaskCopies = 1;

  // But check if source BitMap is interleaved
  if(GetBitMapAttr(pImage, BMA_FLAGS) & BMF_INTERLEAVED)
  {
    // Source BitMap is interleaved; must copy mask m_Depth times
    numMaskCopies = m_Depth;
  }

  m_MaskSizeBytes = planeSizeBytes * numMaskCopies;

  // Allocate memory for the temporary planar mask
  pMaskPlanar = (UBYTE*) AllocVec(planeSizeBytes, MEMF_ANY|MEMF_CLEAR);
  if(pMaskPlanar == NULL)
  {
    throw "ShadowMaskInterleaved: Failed to allocate memory for (temporary) planar mask";
  }
  
  // Allocate memory for the final mask (m_Depth * m_Height because interleaved)
  m_pMask = (UBYTE*)AllocVec(m_MaskSizeBytes, MEMF_CHIP|MEMF_CLEAR);
  if(m_pMask == NULL)
  {
    FreeVec(pMaskPlanar);
    throw "ShadowMaskInterleaved: Failed to allocate memory for interleaved mask.";
  }

  // Create the planar mask
  for (i = 0; i < planeSizeBytes; i++)
  {
    maskedByte = 0;

    for (j = 0; j < m_Depth; j++)
    {
      pPlane = pImage->Planes[j];
      maskedByte |= pPlane[i];
    }

    pMaskPlanar[i] = maskedByte;
  }


  // Convert the planar mask into the interleaved mask
  bytesPerRow = m_WordWidth * 2;
  iSrcRow = 0;
  iDstRow = 0;
  do
  {
    for(iSrcCol = 0; iSrcCol < bytesPerRow; iSrcCol++)
    {
      // Address the correct byte in planar source mask
      // pSrcByte = pMaskBitMap->Planes[0] + ((iSrcRow * bytesPerRow) + iSrcCol);
      pSrcByte = pMaskPlanar + ((iSrcRow * bytesPerRow) + iSrcCol);

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

  FreeVec(pMaskPlanar);
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