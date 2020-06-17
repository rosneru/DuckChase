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

  m_pRowPixels = new bool[m_WordWidth * 2 * 8];

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
  m_pRowPixels = new bool[m_WordWidth * 2 * 8];
}

ShadowMask::~ShadowMask()
{
  delete[] m_pRowPixels;

  if(m_pMask != NULL)
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

    // Now compare the pixels of this row of both collision rects
    for(size_t pixelId = 0; pixelId <= thisRect.Width(); pixelId++)
    {
      if((this->m_pRowPixels[pixelId] == true) &&
         (pOther->m_pRowPixels[pixelId] == true))
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
