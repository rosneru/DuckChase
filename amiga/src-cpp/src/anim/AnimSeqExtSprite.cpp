#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <exec/memory.h>

#include <stdio.h>
#include <string.h>

#include "AnimSeqExtSprite.h"
#include "IlbmBitmap.h"

AnimSeqExtSprite::AnimSeqExtSprite(const char* pFileName, 
                                   size_t numFrames)
  : AnimSeqBase(numFrames),
    m_pFrameBitMap(NULL),
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

  // Copy the first n colors from source image (with n = depth)
  const ULONG* pSrcImgColors = srcIlbmBitmap.GetColors32();
  if(pSrcImgColors != NULL)
  {
    size_t numColors = 1L << srcIlbmBitmap.Depth();

    // Size of the Colors32 table (number of ULONG values)
    size_t colorArraySize = 3 * numColors + 2;

    // Size of the Colors32 table in bytes
    size_t colArrByteSize = colorArraySize * sizeof(ULONG);

    // Alloc color table
    m_pColors32 = (ULONG*) AllocVec(colArrByteSize, MEMF_ANY|MEMF_CLEAR);
    if(m_pColors32 == NULL)
    {
      throw "AnimSeqExtSprite: Failed to alloc memory for colors.";
    }

    // Copy starting part of the src color map to dest
    CopyMem((APTR)pSrcImgColors, m_pColors32, colArrByteSize);

    // LoadRGB32() needs the number of colors to load in the higword
    // (the left 16 bit) of the color table's first ULONG value
    m_pColors32[0] = numColors << 16;

    // Finalize the color array
    m_pColors32[colorArraySize-1] = 0ul;
  }

  // Create a dynamic array for all frames
  size_t arraySize = numFrames * sizeof(struct ExtSprite*);
  m_ppFrames = (struct ExtSprite**)AllocVec(arraySize, MEMF_ANY);
  if (m_ppFrames == NULL)
  {
    throw "AnimSeqExtSprite: Failed to allocate frame array memory.";
  }

  // Create a destination BitMap to blit each destination frame image into
  size_t frameWidth = srcIlbmBitmap.Width() / numFrames;
  m_pFrameBitMap = AllocBitMap(frameWidth,
                               srcIlbmBitmap.Height(),
                               srcIlbmBitmap.Depth(),
                               BMF_CLEAR,
                               NULL);

  // Create all frames by copying an area of src bitmap
  for(size_t i = 0; i < numFrames; i++)
  {

    size_t xStart = i * frameWidth;
    BltBitMap(srcIlbmBitmap.GetBitMap(),
              xStart,
              0,
              m_pFrameBitMap,
              0,
              0,
              frameWidth - 1,
              srcIlbmBitmap.Height() - 1,
              0Xc0,
              0xff,
              NULL);

    struct ExtSprite* pSpriteImage = AllocSpriteData(m_pFrameBitMap, 
                                                     SPRITEA_Width, frameWidth, 
                                                     TAG_DONE);

    if (pSpriteImage == NULL)
    {
      throw "AnimSeqExtSprite: Failed to allocate sprite data.";
    }

    m_ppFrames[i] = pSpriteImage;
    m_ppShadowMasks[i] = new ShadowMask(m_pFrameBitMap);
  }

  FreeBitMap(m_pFrameBitMap);
  m_pFrameBitMap = NULL;
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

