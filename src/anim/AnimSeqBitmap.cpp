#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <exec/memory.h>

#include <stdio.h>
#include <string.h>

#include "AnimSeqBitmap.h"
#include "OpenIlbmPictureBitMap.h"

AnimSeqBitmap::AnimSeqBitmap(const char* pFileName, 
                                   size_t numFrames)
  : AnimSeqBase(numFrames),
    m_ppFrames(NULL)
{
  if (pFileName == NULL)
  {
    throw "AnimSeqBitMap: No file name provided.";
  }

  // Load the src image file into a bitmap
  OpenIlbmPictureBitMap srcIlbmBitmap(pFileName, true, false);

  m_Width = srcIlbmBitmap.Width() / numFrames;
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
  m_Height = srcIlbmBitmap.Height();
  m_Depth = srcIlbmBitmap.Depth();

  // Create a dynamic array for all frames
  size_t arraySize = numFrames * sizeof(struct BitMap*);
  m_ppFrames = (struct BitMap**)AllocVec(arraySize, MEMF_ANY);
  if (m_ppFrames == NULL)
  {
    throw "AnimSeqBitMap: Failed to allocate array memory.";
  }

  // Create a destination BitMap to blit each destination frame image into
  size_t frameWidth = srcIlbmBitmap.Width() / numFrames;

  // Create all frames by copying an area of src bitmap
  for(size_t i = 0; i < numFrames; i++)
  {
    struct BitMap* pFrameBitMap = AllocBitMap(frameWidth,
                                                srcIlbmBitmap.Height(),
                                                srcIlbmBitmap.Depth(),
                                                BMF_CLEAR,
                                                NULL);
    if (pFrameBitMap == NULL)
    {
      throw "AnimSeqBitMap: Failed to allocate sprite data.";
    }

    size_t xStart = i * frameWidth;
    BltBitMap(srcIlbmBitmap.GetBitMap(),
              xStart,
              0,
              pFrameBitMap,
              0,
              0,
              frameWidth,
              srcIlbmBitmap.Height(),
              0Xc0,
              0xff,
              NULL);

    m_ppFrames[i] = pFrameBitMap;
  }
}

AnimSeqBitmap::~AnimSeqBitmap()
{
  if (m_ppFrames != NULL)
  {
    for (size_t i = 0; i < m_NumFrames; i++)
    {
      if (m_ppFrames[i] != NULL)
      {
        FreeBitMap(m_ppFrames[i]);
        m_ppFrames[i] = NULL;
      }
    }

    FreeVec(m_ppFrames);
    m_ppFrames = NULL;
  }
}


BitMap* AnimSeqBitmap::operator[](size_t index) const
{
  if(index >= m_NumFrames)
  {
    return NULL;
  }

  return m_ppFrames[index];
}
