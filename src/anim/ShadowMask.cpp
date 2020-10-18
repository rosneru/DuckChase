#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include <stdio.h>

#include "ShadowMask.h"

ShadowMask::ShadowMask(const struct BitMap* pImage)
  : m_pMask(NULL),
    m_IsForeignMask(false)  // The mask is created here. It is *not* foreign.
{
  m_Width = GetBitMapAttr(pImage, BMA_WIDTH);
  m_Height = GetBitMapAttr(pImage, BMA_HEIGHT);
  size_t depth = GetBitMapAttr(pImage, BMA_DEPTH);

  m_WordWidth = ((m_Width + 15) & -16) >> 4;

  size_t numBytes = pImage->BytesPerRow * pImage->Rows;
  m_pMask = (UBYTE*)AllocVec(numBytes, MEMF_CLEAR|MEMF_ANY); // TODO MEMF_CHIP for Blitter use
  if(m_pMask == NULL)
  {
    throw "ShadowMask: Failed to allocate memory for mask.";
  }

  m_pRowPixels = new bool[m_WordWidth * 2 * 8];

  for (size_t i = 0; i < numBytes; i++)
  {
    UBYTE maskByte = 0;

    for (size_t j = 0; j < depth; j++)
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
    m_IsForeignMask(true),  // The mask has been passed from outside. It *is* foreign.
    m_Width(width),
    m_WordWidth(((width + 15) & -16) >> 4),
    m_Height(height)
{
  m_pRowPixels = new bool[m_WordWidth * 2 * 8];
}

ShadowMask::~ShadowMask()
{
  delete[] m_pRowPixels;

  if((!m_IsForeignMask) && (m_pMask != NULL))
  {
    FreeVec(m_pMask);
    m_pMask = NULL;
  }
}


bool ShadowMask::IsCollision(const ShadowMask* pOther, 
                             const Rect& thisRect,
                             const Rect& otherRect) const
{
  size_t thisRow = thisRect.Top();
  size_t thisStopRow = thisRect.Bottom();
  size_t otherRow = otherRect.Top();

  do
  {
    // Calculate which pixels in this mask are set in this row
    this->calculateRowPixels(thisRect, thisRow);

    // Calculate which pixels in the other mask are set in this row
    pOther->calculateRowPixels(otherRect, otherRow);

    // Now compare this rows pixels of both collision rects
    for(size_t column = 0; column <= thisRect.Width(); column++)
    {
      if((this->m_pRowPixels[column] == true) &&
         (pOther->m_pRowPixels[column] == true))
      {
        // Pixel is set in both collsion rects -> Collision
        return true;
      }
    }

    thisRow++;
    otherRow++;
  }
  while(thisRow <= thisStopRow);



  return false;
}


void ShadowMask::Print()
{
  ULONG bytesPerRow = m_WordWidth * 2;
  PLANEPTR pPlane = m_pMask;
  for(size_t row = 0; row < m_Height; row++)
  {
    for(int byte = 0; byte < bytesPerRow; byte++)
    {
      size_t offset = byte + (bytesPerRow * row);
      printBits(1, pPlane + offset);
      printf(" ");
    }
    printf("\n");
  }
  printf("\n");
}


void ShadowMask::calculateRowPixels(const Rect& rect, size_t row) const
{
  for(int column = rect.Left(); column <= rect.Right(); column++)
  {
    size_t rowByte = column >> 3;
    size_t byteId = row * (m_WordWidth * 2) + rowByte;
    size_t bitInByte = 7 - (column - rowByte * 8);
    UBYTE byteValue = m_pMask[byteId];
    size_t bitValue = byteValue & (1 << bitInByte);

    if(bitValue != 0)
    {
      m_pRowPixels[column - rect.Left()] = true;
    }
    else
    {
      m_pRowPixels[column - rect.Left()] = false;
    }
  }
}

// assumes little endian
void ShadowMask::printBits(size_t const size, void const* const ptr)
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