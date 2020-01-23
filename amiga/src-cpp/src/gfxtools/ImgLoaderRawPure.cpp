#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <dos/dos.h>
#include <exec/memory.h>

#include "ImgLoaderRawPure.h"

ImgLoaderRawPure::ImgLoaderRawPure(int width, int height, int depth)
  : ImgLoaderBase(width, height, depth),
    m_pImageData(NULL)
{

}

ImgLoaderRawPure::~ImgLoaderRawPure()
{
  if(m_pImageData != NULL)
  {
    FreeVec(m_pImageData);
    m_pImageData = NULL;
  }
}

bool ImgLoaderRawPure::Load(const char* pFileName)
{
  if (pFileName == NULL)
  {
    setErrorMsg(m_pInternalError);
    return false;
  }

  BPTR fileHandle = Open(pFileName, MODE_OLDFILE);
  if (fileHandle == 0)
  {
    setErrorMsg(m_pLoadError, pFileName);
    return false;
  }

  // Determine needed memory size for image data
  LONG bufSizeBytes = m_WordWidth * 2 * m_Height * m_Depth;

  if(m_pImageData == NULL)
  {
    m_pImageData = (WORD*) AllocVec(bufSizeBytes, MEMF_CHIP|MEMF_CLEAR);
    if(m_pImageData == NULL)
    {
      setErrorMsg(m_pAllocError);
      Close(fileHandle);
      return false;
    }
  }

  // Read the file data into target chip memory buffer
  if (Read(fileHandle, m_pImageData, bufSizeBytes) != bufSizeBytes)
  {
    // Error while reading
    setErrorMsg(m_pLoadError, pFileName);
    Close(fileHandle);
    FreeVec(m_pImageData);
    m_pImageData = NULL;
    return false;
  }

  Close(fileHandle);
  return true;
}

WORD* ImgLoaderRawPure::GetImageData()
{
  return m_pImageData;
}
