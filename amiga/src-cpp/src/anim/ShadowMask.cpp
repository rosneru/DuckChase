#include <stddef.h>

#include "ShadowMask.h"

ShadowMask::ShadowMask(const struct BitMap* pImage)
  : m_pMask(NULL)
{
  size_t numBytes = pImage->BytesPerRow * pImage->Rows;
  m_pMask = (UBYTE*)AllocVec(numBytes, MEMF_CLEAR|MEMF_ANY); // TODO MEMF_CHIP for Blitter use
  if(m_pMask == NULL)
  {
    throw "ShadowMask: Failed to allocate memory for mask.";
  }

  m_WordWidth = numBytes / 2;
  m_Width = m_WordWidth * 16;
  m_Height = pImage->Rows;

  for (size_t i = 0; i < numBytes; i++)
  {
    UBYTE maskByte = 0;

    for (size_t j = 0; j < pImage->Depth; j++)
    {
      UBYTE *plane = pImage->Planes[j];
      maskByte |= plane[i];
    }

    m_pMask[i] = maskByte;
  }
}

ShadowMask::ShadowMask(UBYTE* pMask, 
                       ULONG width, 
                       ULONG height)
  : m_pMask(pMask),
    m_Width(width),
    m_WordWidth(((width + 15) & -16) >> 4),
    m_Height(height)
{

}

ShadowMask::~ShadowMask()
{
  if(m_pMask != NULL)
  {
    FreeVec(m_pMask);
    m_pMask = NULL;
  }
}

#include <stdio.h>
UBYTE bits[] = {1, 2, 4, 8, 16, 32, 64, 128};

bool ShadowMask::IsCollision(const ShadowMask* pOther, 
                             const Rect& thisRect,
                             const Rect& otherRect)
{
  size_t startRow = thisRect.Top();
  size_t stopRow = thisRect.Bottom();

  size_t bitIndex = 0;

  for(int row = startRow; row <= stopRow; row++)
  {
    for(int column = thisRect.Left(); column <= thisRect.Right(); column++)
    {
      size_t rowByte = column >> 3;
      size_t byteId = row * (m_WordWidth * 2) + rowByte;
      size_t bitInByte = 7 - (column - rowByte);
      UBYTE byteValue = m_pMask[byteId];
      size_t bitValue = (byteValue >> bitInByte) & bits[bitInByte];

      // printf("rowByte = %lu, byteId = %lu, bitInByte = %lu, byteValue = %lu, bitValue = %lu\n", rowByte, byteId, bitInByte, byteValue, bitValue);

      m_RowPixels[bitIndex++] = bitValue == 0 ? false : true;
    }
  }

  return false;
}
