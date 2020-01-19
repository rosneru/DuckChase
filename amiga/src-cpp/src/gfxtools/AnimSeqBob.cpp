#include <stdio.h>
#include <string.h>

#include "AnimSeqBob.h"

AnimSeqBob::AnimSeqBob(int width, int height, int depth)
  : AnimSeqBase(width, height, depth),
    m_ImageCount(0),
    m_pLastError(NULL),
    m_pLoadError("Failed to load bob image ")
{

}

AnimSeqBob::~AnimSeqBob()
{

}

bool AnimSeqBob::Load(const char** ppFileNames)
{
  if(ppFileNames == NULL)
  {
    return false;
  }

  // count items
  while(ppFileNames[m_ImageCount] != NULL)
  {
    if(ppFileNames[++m_ImageCount] == NULL)
    {
      break;
    }
  } 
  
  // printf("ImageCount = %d\n", m_ImageCount);

  return true;
}


const char* AnimSeqBob::LastError()
{
  return m_pLastError;
}
