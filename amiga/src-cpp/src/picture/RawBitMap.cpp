#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/iffparse_protos.h>
#include <dos/dos.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>

#include "RawBitMap.h"

extern struct GfxBase* GfxBase;


RawBitMap::RawBitMap(const char* pFileName,
                     ULONG width, 
                     ULONG height, 
                     ULONG depth)
  : BitMapPictureBase(),
    m_FileHandle(0)
{
  if (pFileName == NULL)
  {
    throw "RawBitMap: No file name provided.";
  }

  m_FileHandle = Open(pFileName, MODE_OLDFILE);
  if (m_FileHandle == 0)
  {
    throw "RawBitMap: Failed to open file.";
  }

  ULONG flags = BMF_CLEAR; //|BMF_INTERLEAVED;
  m_pBitMap = AllocBitMap(width, height, depth, flags, NULL);

  if (m_pBitMap == NULL)
  {
      throw "RawBitMap: Failed to allocate the BitMap for file.";
  }

  // Check if we really got an interleaved bitmap
  flags = GetBitMapAttr(m_pBitMap, BMA_FLAGS);
  if (flags & BMF_INTERLEAVED)
  {
    // We got an interleaved BitMap, so reading all data at once
    int planeSize = RASSIZE(width, height) * depth;
    if (Read(m_FileHandle, m_pBitMap->Planes[0], planeSize) != planeSize)
    {
      throw "RawBitMap: Failed to read file into allocated interleaved BitMap.";
    }
  }
  else
  {
    // No interleaved BitMap, so reading all plane ptrs individually
    int planeSize = RASSIZE(width, height);
    for (ULONG i = 0; i < depth; i++)
    {
      if (Read(m_FileHandle, m_pBitMap->Planes[i], planeSize) != planeSize)
      {
        throw "RawBitMap: Failed to read file into allocated planar BitMap.";
      }
    }
  }

  Close(m_FileHandle);
  m_FileHandle = 0;
}

RawBitMap::~RawBitMap()
{
  if(m_FileHandle != 0)
  {
    Close(m_FileHandle);
  }
}
