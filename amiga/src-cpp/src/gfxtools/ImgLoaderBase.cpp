#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <dos/dos.h>
#include <exec/memory.h>

#include <string.h>

#include "ImgLoaderBase.h"

ImgLoaderBase::ImgLoaderBase(int width, int height, int depth)
  : m_Width(width),
    m_Height(height),
    m_Depth(depth),
    m_pErrorMsg(NULL),
    m_pAllocError("Failed to alloc memory for anim sequence object."),
    m_pInternalError("Internal error in ImgLoaderBase."),
    m_pLoadError("Failed to load file: ")
{
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
}

ImgLoaderBase::~ImgLoaderBase()
{
  freeErrorMsg();
}


int ImgLoaderBase::GetWidth()
{
  return m_Width;
}

int ImgLoaderBase::GetHeight()
{
  return m_Height;
}

int ImgLoaderBase::GetDepth()
{
  return m_Depth;
}

const char* ImgLoaderBase::ErrorMsg()
{
  return m_pErrorMsg;
}


void ImgLoaderBase::setErrorMsg(const char* pMsgPart1, 
                                const char* pMsgPart2)
{
  freeErrorMsg();

  size_t msgLen = strlen(pMsgPart1) + strlen(pMsgPart2) + 1;
  m_pErrorMsg = (char*)AllocVec(msgLen, MEMF_ANY);
  strcpy(m_pErrorMsg, pMsgPart1);
  strcat(m_pErrorMsg, pMsgPart2);
}


void ImgLoaderBase::setErrorMsg(const char* pMsg)
{
  freeErrorMsg();
  
  size_t msgLen = strlen(pMsg) + 1;
  m_pErrorMsg = (char*)AllocVec(msgLen, MEMF_ANY);
  strcpy(m_pErrorMsg, pMsg);
}

void ImgLoaderBase::freeErrorMsg()
{
  if(m_pErrorMsg != NULL)
  {
    FreeVec(m_pErrorMsg);
    m_pErrorMsg = NULL;
  }
}
