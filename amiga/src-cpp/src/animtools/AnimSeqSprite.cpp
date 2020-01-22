#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <exec/memory.h>

#include <stdio.h>
#include <string.h>

#include "AnimSeqSprite.h"
#include "ImgLoaderRawBitMap.h"

AnimSeqSprite::AnimSeqSprite(int width, int height, int depth)
  : AnimSeqBase(width, height, depth),
    m_ppImages(NULL),
    m_ImageCount(0),
    m_CurrentImageId(0)
{
}

AnimSeqSprite::~AnimSeqSprite()
{
  if (m_ppImages != NULL)
  {
    for (size_t i = 0; i < m_ImageCount; i++)
    {
      if (m_ppImages[i] != NULL)
      {
        FreeSpriteData(m_ppImages[i]);
        m_ppImages[i] = NULL;
      }
    }

    FreeVec(m_ppImages);
    m_ppImages = NULL;
  }
}

bool AnimSeqSprite::Load(const char** ppFileNames)
{
  if (ppFileNames == NULL)
  {
    setErrorMsg(m_pInternalError);
    return false;
  }

  if (m_ppImages != NULL)
  {
    // Only one time loading supported
    setErrorMsg(m_pInternalError);
    return false;
  }

  // Count file names
  while (ppFileNames[m_ImageCount] != NULL)
  {
    if (ppFileNames[++m_ImageCount] == NULL)
    {
      break;
    }
  }

  if (m_ImageCount == 0)
  {
    return true; // or false ??
  }

  // Create a dynamic array for all images according to the number of
  // files
  m_ppImages = (struct ExtSprite**) AllocVec(
    m_ImageCount * sizeof(struct ExtSprite*), MEMF_ANY);

  if (m_ppImages == NULL)
  {
    setErrorMsg(m_pAllocError);
    return false;
  }

  // Load all files
  ImgLoaderRawBitMap image(m_Width, m_Height, m_Depth);
  struct ExtSprite* pSpriteImage;

  for (size_t i = 0; i < m_ImageCount; i++)
  {
    if (image.Load(ppFileNames[i]) == false)
    {
      setErrorMsg(image.ErrorMsg());
      return false;
    }

    pSpriteImage = AllocSpriteData(image.GetBitMap(), 
                                  SPRITEA_Width, m_Width,
                                  TAG_DONE);
    if(pSpriteImage == NULL)
    {
      setErrorMsg(m_pAllocError);
      return false;
    }

    m_ppImages[i] = pSpriteImage;
  }

  return true;
}

struct ExtSprite* AnimSeqSprite::GetFirstImage()
{
  if ((m_ppImages == NULL) || (m_ImageCount == 0))
  {
    return NULL;
  }

  // Select the first image
  m_CurrentImageId = 0;

  if (m_ppImages[m_CurrentImageId] == NULL)
  {
    return NULL;
  }

  return m_ppImages[m_CurrentImageId];
}

struct ExtSprite* AnimSeqSprite::GetNextImage()
{
  if ((m_ppImages == NULL) || (m_ImageCount == 0))
  {
    return NULL;
  }

  // Select the next image
  m_CurrentImageId++;
  if (m_CurrentImageId >= m_ImageCount)
  {
    m_CurrentImageId = 0;
  }

  if (m_ppImages[m_CurrentImageId] == NULL)
  {
    return NULL;
  }

  return m_ppImages[m_CurrentImageId];
}
