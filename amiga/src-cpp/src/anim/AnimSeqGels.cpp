#include <clib/exec_protos.h>
#include <exec/memory.h>

#include <stdio.h>
#include <string.h>

#include "ImageDataPicture.h"
#include "AnimSeqGels.h"


AnimSeqGels::AnimSeqGels(size_t numFrames)
  : AnimSeqBase(numFrames),
    m_ppFrames(NULL)
{
}


AnimSeqGels::~AnimSeqGels()
{
  if (m_ppFrames != NULL)
  {
    for (size_t i = 0; i < m_NumFrames; i++)
    {
      if (m_ppFrames[i] != NULL)
      {
        delete m_ppFrames[i];
        m_ppFrames[i] = NULL;
      }
    }

    delete[] m_ppFrames;
    m_ppFrames = NULL;
  }
}

WORD* AnimSeqGels::operator[](size_t index) const
{
  if(index >= m_NumFrames)
  {
    return NULL;
  }

  return m_ppFrames[index]->GetImageData();
}
