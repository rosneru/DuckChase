#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <dos/dos.h>
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


const char* AnimSeqBase::ErrorMsg()
{
  return m_pErrorMsg;
}


WORD* AnimSeqBase::loadRawImageData(const char *pPath)
{
  if (pPath == NULL)
  {
    return NULL;
  }

  BPTR fileHandle = Open(pPath, MODE_OLDFILE);
  if (fileHandle == 0)
  {
    return NULL;
  }

  // Determine needed memory size for image data
  LONG bufSizeBytes = m_WordWidth * 2 * m_Height * m_Depth;

  WORD *pImgData = (WORD*) AllocVec(bufSizeBytes, MEMF_CHIP|MEMF_CLEAR);

  // Read the file data into target chip memory buffer
  if (Read(fileHandle, pImgData, bufSizeBytes) != bufSizeBytes)
  {
    // Error while reading
    Close(fileHandle);
    FreeVec(pImgData);
    return NULL;
  }

  Close(fileHandle);
  return pImgData;
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