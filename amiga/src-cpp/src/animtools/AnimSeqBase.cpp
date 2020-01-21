#include <clib/exec_protos.h>
#include <exec/memory.h>

#include <string.h>

#include "AnimSeqBase.h"

AnimSeqBase::AnimSeqBase(int width, int height, int depth)
  : m_Width(width),
    m_Height(height),
    m_Depth(depth),
    m_pErrorMsg(NULL),
    m_pInternalError("Internal error in AnimSeq."),
    m_pLoadError("Failed to load file: "),
    m_pAllocError("Failed to alloc memory for anim sequence object.")
{
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
}

AnimSeqBase::~AnimSeqBase()
{
  freeErrorMsg();
}

int AnimSeqBase::GetWidth()
{
  return m_Width;
}

int AnimSeqBase::GetWordWidth()
{
  return m_WordWidth;
}

int AnimSeqBase::GetHeight()
{
  return m_Height;
}

int AnimSeqBase::GetDepth()
{
  return m_Depth;
}



const char* AnimSeqBase::ErrorMsg()
{
  return m_pErrorMsg;
}


void AnimSeqBase::setErrorMsg(const char* pMsgPart1, 
                              const char* pMsgPart2)
{
  freeErrorMsg();

  size_t msgLen = strlen(pMsgPart1) + strlen(pMsgPart2) + 1;
  m_pErrorMsg = (char*)AllocVec(msgLen, MEMF_ANY);
  strcpy(m_pErrorMsg, pMsgPart1);
  strcat(m_pErrorMsg, pMsgPart2);
}


void AnimSeqBase::setErrorMsg(const char* pMsg)
{
  freeErrorMsg();
  
  size_t msgLen = strlen(pMsg) + 1;
  m_pErrorMsg = (char*)AllocVec(msgLen, MEMF_ANY);
  strcpy(m_pErrorMsg, pMsg);
}

void AnimSeqBase::freeErrorMsg()
{
  if(m_pErrorMsg != NULL)
  {
    FreeVec(m_pErrorMsg);
    m_pErrorMsg = NULL;
  }
}