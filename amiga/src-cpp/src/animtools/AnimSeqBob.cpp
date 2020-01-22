#include <clib/exec_protos.h>
#include <exec/memory.h>

#include <stdio.h>
#include <string.h>

#include "AnimSeqBob.h"
#include "ImgLoaderRawPure.h"

AnimSeqBob::AnimSeqBob(int width, int height, int depth)
  : AnimSeqBase(width, height, depth),
    m_ppImages(NULL),
    m_ImageCount(0),
    m_CurrentImageId(0)
{
}

AnimSeqBob::~AnimSeqBob()
{
  if (m_ppImages != NULL)
  {
    for (size_t i = 0; i < m_ImageCount; i++)
    {
      if (m_ppImages[i] != NULL)
      {
        delete m_ppImages[i];
        m_ppImages[i] = NULL;
      }
    }

    delete[] m_ppImages;
    m_ppImages = NULL;
  }
}

bool AnimSeqBob::Load(const char** ppFileNames)
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
  m_ppImages = (ImgLoaderRawPure**)new ImgLoaderRawPure*[m_ImageCount];
  if (m_ppImages == NULL)
  {
    setErrorMsg(m_pAllocError);
    return false;
  }

  // Load all files
  for (size_t i = 0; i < m_ImageCount; i++)
  {
    // BOB data are pure RAW data
    ImgLoaderRawPure* pImg = new ImgLoaderRawPure(m_Width, 
                                                  m_Height, 
                                                  m_Depth);

    if (pImg->Load(ppFileNames[i]) == false)
    {
      setErrorMsg(pImg->ErrorMsg());
      return false;
    }

    m_ppImages[i] = pImg;
  }

  return true;
}

WORD* AnimSeqBob::GetFirstImage()
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

  return m_ppImages[m_CurrentImageId]->GetImageData();
}

WORD* AnimSeqBob::GetNextImage()
{
  if ((m_ppImages == NULL) || (m_ImageCount == 0))
  {
    return NULL;
  }

  // Select the next image
  m_CurrentImageId++;
  if(m_CurrentImageId >= m_ImageCount)
  {
    m_CurrentImageId = 0;
  }

  if (m_ppImages[m_CurrentImageId] == NULL)
  {
    return NULL;
  }

  return m_ppImages[m_CurrentImageId]->GetImageData();
}

WORD* AnimSeqBob::GetCurrentImage()
{
  if ((m_ppImages == NULL) || (m_ImageCount == 0))
  {
    return NULL;
  }

  return m_ppImages[m_CurrentImageId]->GetImageData();
}