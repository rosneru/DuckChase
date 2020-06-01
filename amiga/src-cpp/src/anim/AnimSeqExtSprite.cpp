#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <exec/memory.h>

#include <stdio.h>
#include <string.h>

#include "AnimSeqExtSprite.h"
#include "IlbmBitmap.h"

AnimSeqExtSprite::AnimSeqExtSprite(const char* pFileName, 
                                   size_t numFrames)
  : m_pFrameBitMap(NULL),
    m_ppFrames(NULL),
    m_pColors32(NULL)
{
  if (pFileName == NULL)
  {
    throw "AnimSeqExtSprite: No file name provided.";
  }

  // Load the src image file into a bitmap
  IlbmBitmap srcIlbmBitmap(pFileName, true, false);

  m_Width = srcIlbmBitmap.Width() / numFrames;
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
  m_Height = srcIlbmBitmap.Height();
  m_Depth = srcIlbmBitmap.Depth();

  // Copy the first depth colors from source image
  ULONG* pSrcImgColors = srcIlbmBitmap.GetColors32();
  if(pSrcImgColors != NULL)
  {
    size_t numColors = 1L << srcIlbmBitmap.Depth();
    size_t colArrSize = ((2 + 3 * numColors) * sizeof(ULONG));
    m_pColors32 = (ULONG*) AllocVec(colArrSize, MEMF_ANY);
    if(m_pColors32 == NULL)
    {
      throw "AnimSeqExtSprite: Failed to alloc memory for colors.";
    }

    // Copy starting part of the src color map to dest
    CopyMem(pSrcImgColors, m_pColors32, colArrSize);

    // Write the actual number of colors at position 0 for LoadRGB32()
    m_pColors32[0] = numColors << 16;

    // Finalize the color array
    m_pColors32[colArrSize-1] = 0L;
  }

  // Create a dynamic array for all frames
  size_t arraySize = numFrames * sizeof(struct ExtSprite*);
  m_ppFrames = (struct ExtSprite**)AllocVec(arraySize, MEMF_ANY);
  if (m_ppFrames == NULL)
  {
    throw "AnimSeqExtSprite: Failed to allocate array memory.";
  }

  // Create a destination BitMap to blit each destination frame image into
  size_t frameWidth = srcIlbmBitmap.Width() / numFrames;
  m_pFrameBitMap = AllocBitMap(frameWidth,
                               srcIlbmBitmap.Height(),
                               srcIlbmBitmap.Depth(),
                               BMF_CLEAR,
                               NULL);

  // Create all frames by copying an area of src bitmap
  struct ExtSprite* pSpriteImage = NULL;
  for(size_t i = 0; i < numFrames; i++)
  {

    size_t xStart = i * frameWidth;
    BltBitMap(srcIlbmBitmap.GetBitMap(),
              xStart,
              0,
              m_pFrameBitMap,
              0,
              0,
              frameWidth,
              srcIlbmBitmap.Height(),
              0Xc0,
              0xff,
              NULL);

    pSpriteImage = AllocSpriteData(m_pFrameBitMap, 
                                   SPRITEA_Width, frameWidth, 
                                   TAG_DONE);

    if (pSpriteImage == NULL)
    {
      throw "AnimSeqExtSprite: Failed to allocate sprite data.";
    }

    m_ppFrames[i] = pSpriteImage;
  }

  FreeBitMap(m_pFrameBitMap);
  m_pFrameBitMap = NULL;

  m_NumFrames = numFrames;
}

AnimSeqExtSprite::~AnimSeqExtSprite()
{
  if(m_pFrameBitMap != NULL)
  {
    FreeBitMap(m_pFrameBitMap);
  }

  if (m_ppFrames != NULL)
  {
    for (size_t i = 0; i < m_NumFrames; i++)
    {
      if (m_ppFrames[i] != NULL)
      {
        FreeSpriteData(m_ppFrames[i]);
        m_ppFrames[i] = NULL;
      }
    }

    FreeVec(m_ppFrames);
    m_ppFrames = NULL;
  }

  if(m_pColors32 != NULL)
  {
    FreeVec(m_pColors32);
    m_pColors32 = NULL;
  }
}


ExtSprite* AnimSeqExtSprite::operator[](size_t index) const
{
  if(index >= m_NumFrames)
  {
    return NULL;
  }

  return m_ppFrames[index];
}

ULONG* AnimSeqExtSprite::GetColors32() const
{
  return m_pColors32;
}
