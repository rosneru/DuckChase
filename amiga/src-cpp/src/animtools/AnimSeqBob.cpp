#include <clib/exec_protos.h>
#include <exec/memory.h>

#include <stdio.h>
#include <string.h>

#include "AnimSeqBob.h"

AnimSeqBob::AnimSeqBob(int width, int height, int depth)
  : AnimSeqBase(width, height, depth),
    m_ppImages(NULL),
    m_ImageCount(0)
{

}

AnimSeqBob::~AnimSeqBob()
{
  if(m_ppImages != NULL)
  {
    for(size_t i = 0; i < m_ImageCount; i++)
    {
      if(m_ppImages[i] != NULL)
      {
        FreeVec(m_ppImages[i]);
        m_ppImages[i] = NULL;
      }
    }

    FreeVec(m_ppImages);
    m_ppImages = NULL;
  }
}

bool AnimSeqBob::Load(const char** ppFileNames)
{
  if(ppFileNames == NULL)
  {
    setErrorMsg(m_pInternalError);
    return false;
  }

  if(m_ppImages != NULL)
  {
    // Only one time loading supported
    setErrorMsg(m_pInternalError);
    return false;
  }

  // Count file names
  while(ppFileNames[m_ImageCount] != NULL)
  {
    if(ppFileNames[++m_ImageCount] == NULL)
    {
      break;
    }
  } 

  if(m_ImageCount == 0)
  {
    return true; // or false ??
  }
  
  // Create dynamic array for all images according to the number of files
  m_ppImages = (WORD**) AllocVec(m_ImageCount * sizeof(WORD*), 
                                 MEMF_CLEAR);
  if(m_ppImages == NULL)
  {
    setErrorMsg(m_pAllocError);
    return false;
  }

  // Load all files, for BOB data thei're supposed to be RAW
  for(size_t i = 0; i < m_ImageCount; i++)
  {
    m_ppImages[i] = loadRawImageData(ppFileNames[i]);
    if(m_ppImages[i] == NULL)
    {
      setErrorMsg(m_pLoadError, ppFileNames[i]);
      return false;
    }
  }

  return true;
}
