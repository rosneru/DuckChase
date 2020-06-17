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
    m_WordWidth(),
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

bool ShadowMask::IsCollision(const ShadowMask* pOther, 
                             const Rect& thisRect,
                             const Rect& otherRect) const
{
  return false;
}
