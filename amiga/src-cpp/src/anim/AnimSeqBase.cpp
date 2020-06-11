#include <clib/exec_protos.h>
#include <exec/memory.h>

#include <string.h>

#include "AnimSeqBase.h"

AnimSeqBase::AnimSeqBase(size_t numFrames)
  : m_NumFrames(numFrames),
    m_ppShadowMasks(NULL)
{
  // Create the shadow mask array
  m_ppShadowMasks = (UBYTE**) AllocVec(numFrames * sizeof(UBYTE**), 
                                       MEMF_ANY|MEMF_CLEAR);
  if (m_ppShadowMasks == NULL)
  {
    throw "AnimSeqBase: Failed to allocate memory for shadow mask array.";
  }
}

AnimSeqBase::~AnimSeqBase()
{
  if(m_ppShadowMasks != NULL)
  {
    for (size_t i = 0; i < m_NumFrames; i++)
    {
      if(m_ppShadowMasks[i] != NULL)
      {
        FreeVec(m_ppShadowMasks[i]);
        m_ppShadowMasks[i] = NULL;
      }
    }
  }
}

int AnimSeqBase::Width() const
{
  return m_Width;
}

int AnimSeqBase::WordWidth() const
{
  return m_WordWidth;
}

int AnimSeqBase::Height() const
{
  return m_Height;
}

int AnimSeqBase::Depth() const
{
  return m_Depth;
}

size_t AnimSeqBase::NumFrames() const
{
  return m_NumFrames;
}

const UBYTE* AnimSeqBase::ShadowMask(size_t index) const
{
  if((index >= m_NumFrames) || (m_ppShadowMasks == NULL))
  {
    return NULL;
  }

  return m_ppShadowMasks[index];
}

UBYTE* AnimSeqBase::createShadowMask(const struct BitMap* pImage)
{
  // TODO: Does this work for chunky BitMaps?
  size_t numBytes = pImage->BytesPerRow * pImage->Rows;
  UBYTE* pMask = (UBYTE*)AllocVec(numBytes, MEMF_CLEAR|MEMF_ANY); // TODO MEMF_CHIP for Blitter use

  for (size_t i = 0; i < numBytes; i++)
  {
    UBYTE maskByte = 0;

    for (size_t j = 0; j < pImage->Depth; j++)
    {
      UBYTE *plane = pImage->Planes[j];
      maskByte |= plane[i];
    }

    pMask[i] = maskByte;
  }

  return pMask;
}
