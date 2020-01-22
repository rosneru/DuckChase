#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <dos/dos.h>
#include <exec/memory.h>
#include <graphics/gfx.h>

#include "ImgLoaderRawBitMap.h"

ImgLoaderRawBitMap::ImgLoaderRawBitMap(int width, int height, int depth)
  : ImgLoaderBase(width, height, depth),
    m_pBitMap(NULL)
{

}

ImgLoaderRawBitMap::~ImgLoaderRawBitMap()
{
  if(m_pBitMap != NULL)
  {
    FreeBitMap(m_pBitMap);
    m_pBitMap = NULL;
  }
}

bool ImgLoaderRawBitMap::Load(const char* pFileName)
{
  if (pFileName == NULL)
  {
    setErrorMsg(m_pInternalError);
    return false;
  }

  if(m_pBitMap != NULL)
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

  ULONG flags = BMF_CLEAR; //|BMF_INTERLEAVED;
  struct BitMap *pBitMap = AllocBitMap(m_Width, 
                                       m_Height, 
                                       m_Depth, 
                                       flags, 
                                       NULL);

  if (pBitMap == NULL)
  {
    // Couldn't get BitMap
    setErrorMsg(m_pAllocError);
    Close(fileHandle);
    return false;
  }

  // Check if we really got an interleaved bitmap
  flags = GetBitMapAttr(pBitMap, BMA_FLAGS);
  if (flags & BMF_INTERLEAVED)
  {
    // We got an interleaved BitMap, so reading all data at once
    int planeSize = RASSIZE(m_Width, m_Height) * m_Depth;
    if (Read(fileHandle, pBitMap->Planes[0], planeSize) != planeSize)
    {
      // Error while reading
      setErrorMsg(m_pLoadError, pFileName);
      FreeBitMap(pBitMap);
      Close(fileHandle);
      return false;
    }
  }
  else
  {
    // No interleaved BitMap, so reading all plane ptrs individually
    int planeSize = RASSIZE(m_Width, m_Height);
    for (int i = 0; i < m_Depth; i++)
    {
      if (Read(fileHandle, pBitMap->Planes[i], planeSize) != planeSize)
      {
        // Error while reading
        setErrorMsg(m_pLoadError, pFileName);
        FreeBitMap(pBitMap);
        Close(fileHandle);
        return false;
      }
    }
  }

  Close(fileHandle);
  return true;
}

struct BitMap* ImgLoaderRawBitMap::GetBitMap()
{
  return m_pBitMap;
}
