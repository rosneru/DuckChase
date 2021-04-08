#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include <graphics/gfx.h>

#include <stdio.h>

#include "ShadowMaskInterleaved.h"

ShadowMaskInterleaved::ShadowMaskInterleaved(struct BitMap* pImage)
  : m_pMask(NULL),
    m_MaskSizeBytes(0)
{
  UBYTE maskByteValue;
  ULONG byteWidth, bytesPerRow, planeSizeBytes, iSrcPlaneByte, iDstMaskByte, iRow, iByte, iPlane, i;
  UBYTE* pBitplaneData;

  if(pImage == NULL)
  {
    throw "ShadowMaskInterleaved: No BitMap image given.";
  }

  m_Width = GetBitMapAttr(pImage, BMA_WIDTH);
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
  m_Height = GetBitMapAttr(pImage, BMA_HEIGHT);
  m_Depth = GetBitMapAttr(pImage, BMA_DEPTH);

  byteWidth = m_WordWidth * 2;
  planeSizeBytes = RASSIZE(m_Width, m_Height);

  // But check if source BitMap is interleaved
  if((GetBitMapAttr(pImage, BMA_FLAGS) & BMF_INTERLEAVED) == 0)
  {
    // Source BitMap must be interleaved.
    throw "ShadowMaskInterleaved: This mask creation algorithm only works for interleaved BitMaps.";
  }

  // mask must be replicated for each src Bitmap plane
  m_MaskSizeBytes = planeSizeBytes * m_Depth;

  // Allocate memory for the final mask (m_Depth * m_Height because interleaved)
  m_pMask = (UBYTE*)AllocVec(m_MaskSizeBytes, MEMF_CHIP|MEMF_CLEAR);
  if(m_pMask == NULL)
  {
    throw "ShadowMaskInterleaved: Failed to allocate memory for interleaved mask.";
  }

  pBitplaneData = pImage->Planes[0];
  bytesPerRow = pImage->BytesPerRow;  // BytesPerRow in interleaved image
  iSrcPlaneByte = 0;
  iDstMaskByte = 0;
  for(iRow = 0; iRow < m_Height; iRow++)
  {
    for(iByte = 0; iByte < byteWidth; iByte++)
    {
      // Create the mask value for this byte by or'ing over all planes
      maskByteValue = 0;
      iSrcPlaneByte = iRow * bytesPerRow + iByte;
      iDstMaskByte = iSrcPlaneByte; // also the start of the destination mask byte
      for(iPlane = 0; iPlane < m_Depth; iPlane++)
      {
        maskByteValue |= pBitplaneData[iSrcPlaneByte];
        iSrcPlaneByte += byteWidth;
      }

      // Set mask byte into destination mask and replicate 'depth' times
      for(i = 0; i < m_Depth; i++)
      {
        m_pMask[iDstMaskByte] = maskByteValue;
        iDstMaskByte += byteWidth;
      }
    }
  }
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
  for(ULONG i = 0; i < m_MaskSizeBytes / m_Depth; i++)
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