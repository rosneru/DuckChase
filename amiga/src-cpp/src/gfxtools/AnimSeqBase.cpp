#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <dos/dos.h>
#include <exec/memory.h>

#include "AnimSeqBase.h"

AnimSeqBase::AnimSeqBase(int width, int height, int depth)
  :  m_Width(width),
     m_Height(height),
     m_Depth(depth)
{
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
}

AnimSeqBase::~AnimSeqBase()
{

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
