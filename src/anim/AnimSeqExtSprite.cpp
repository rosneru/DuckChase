#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <exec/memory.h>

#include <stdio.h>
#include <string.h>

#include "AnimSeqExtSprite.h"
#include "OpenIlbmPictureBitMap.h"

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
  OpenIlbmPictureBitMap srcIlbmBitmap(pFileName, true, false);

  m_Width = srcIlbmBitmap.Width() / numFrames;
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
  m_Height = srcIlbmBitmap.Height();
  m_Depth = srcIlbmBitmap.Depth();

  // Copy the color table from source picture
  m_pColors32 = deepCopyColors(srcIlbmBitmap);
  if(m_pColors32 == NULL)
  {
    throw "Failed to copy color table for sprite anim.";
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
  
  if(m_pFrameBitMap == NULL)
  {
    throw "Failed to create temporary Bitmap.";
  }

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
              frameWidth,
              srcIlbmBitmap.Height(),
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

ULONG* AnimSeqExtSprite::deepCopyColors(const BitMapPictureBase& srcPicture)
{
  // Copy the first n colors from source image (with n = depth)
  ULONG* pSrcBitMapColors32 = srcPicture.GetColors32();
  if(pSrcBitMapColors32 == NULL)
  {
    return NULL;
  }

  size_t numColors = 1L << srcPicture.Depth();

  // Size of the Colors32 table (number of ULONG values)
  size_t colorArraySize = 3 * numColors + 2;

  // Size of the Colors32 table in bytes
  size_t colorArrayByteSize = colorArraySize * sizeof(ULONG);

  // Alloc color table
  ULONG* pColors32 = (ULONG*) AllocVec(colorArrayByteSize, MEMF_ANY|MEMF_CLEAR);
  if(pColors32 == NULL)
  {
    return NULL;
  }

  // Copy starting part of the src color map to dest
  CopyMem((APTR)pSrcBitMapColors32, pColors32, colorArrayByteSize);

  // LoadRGB32() needs the number of colors to load in the higword
  // (the left 16 bit) of the color table's first ULONG value
  pColors32[0] = numColors << 16;

  // Finalize the color array
  pColors32[colorArraySize-1] = 0ul;

  return pColors32;
}