#include <clib/exec_protos.h>
#include <clib/datatypes_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>

#include <graphics/gfx.h>

#include "Picture.h"


Picture::Picture()
  : m_pObject(NULL),
    m_pBitMapDtHeader(NULL),
    m_pPaletteDt(NULL),
    m_pBitMapDt(NULL),
    m_pBitMapRaw(NULL),
    m_pPlaneMemoryRaw(NULL),
    m_NumberOfColors(0)
{
}


Picture::~Picture()
{
  clear();
}


bool Picture::LoadFromDatatype(const char* p_pFilePath,
                               Screen* p_pScreen)
{
  // TODO Check for at least OS3.0 / v39 and return false otherwise

  clear();

  m_pObject = NewDTObject((void*)p_pFilePath,
                          DTA_GroupID, GID_PICTURE,
                          PDTA_Remap, FALSE,
                          PDTA_Screen, p_pScreen,
                          TAG_END);

  if (m_pObject == NULL)
  {
    return false;
  }

  DoDTMethod(m_pObject, NULL, NULL, DTM_PROCLAYOUT, NULL, TRUE);

  GetDTAttrs(m_pObject,
             PDTA_NumColors, &m_NumberOfColors,
             PDTA_CRegs, &m_pPaletteDt,
             PDTA_BitMapHeader, &m_pBitMapDtHeader,
             PDTA_DestBitMap, &m_pBitMapDt,
             TAG_END);

  return true;
}

bool Picture::LoadFromRawFile(const char* p_pPath,
                              int p_ImageWidth,
                              int p_ImageHeight,
                              short p_ImageDepth)
{
  BPTR fileHandle = Open(p_pPath, MODE_OLDFILE);
  if (fileHandle == NULL)
  {
    return false;
  }

  // Clear formerly used data
  clear();

  // Calculate the size in bytes needed for one bitplane of given
  // dimension
  int planeSize = RASSIZE(p_ImageWidth, p_ImageHeight);
  int bufSize = planeSize * p_ImageDepth;

  m_pPlaneMemoryRaw = (UBYTE*) AllocVec(bufSize, MEMF_CHIP|MEMF_CLEAR);
  if(m_pPlaneMemoryRaw == NULL)
  {
    // Not enough memory
    Close(fileHandle);
    return false;
  }

  // Read the file data into target bit plane buffer
  if (Read(fileHandle, m_pPlaneMemoryRaw, bufSize) != bufSize)
  {
    // Error while reading
    Close(fileHandle);
    return false;
  }

  Close(fileHandle);

  int bmSize = sizeof(struct BitMap);
  m_pBitMapRaw = (struct BitMap*) AllocVec(bmSize, MEMF_CLEAR);
  if(m_pBitMapRaw == NULL)
  {
    // Not enough memory
    return false;
  }

  InitBitMap(m_pBitMapRaw, p_ImageDepth, p_ImageWidth, p_ImageHeight);

  // Set each plane pointer to its dedicated portion of the data
  UBYTE* pPlanePtr = m_pPlaneMemoryRaw;
  for(int i = 0; i < p_ImageDepth; i++)
  {
    m_pBitMapRaw->Planes[i] = pPlanePtr;
    pPlanePtr += planeSize;
  }

  return true;
}

struct BitMap* Picture::GetBitMap()
{
  if(m_pBitMapDt != NULL)
  {
    return m_pBitMapDt;
  }

  if(m_pBitMapRaw != NULL)
  {
    return m_pBitMapRaw;
  }

  return NULL;
}

struct BitMapHeader* Picture::GetBitMapHeader()
{
  return m_pBitMapDtHeader;
}


ULONG* Picture::GetPalette()
{
  return m_pPaletteDt;
}


long Picture::GetNumberOfColors()
{
  return m_NumberOfColors;
}


void Picture::clear()
{
  if(m_pObject)
  {
    DisposeDTObject(m_pObject);
    m_pObject = NULL;
    m_pBitMapDt = NULL;
    m_pBitMapDtHeader = NULL;
    m_pPaletteDt = NULL;
    m_NumberOfColors = 0;
  }

  if(m_pBitMapRaw != NULL)
  {
    FreeVec(m_pBitMapRaw);
    m_pBitMapRaw = NULL;
  }

  if(m_pPlaneMemoryRaw != NULL)
  {
    FreeVec(m_pPlaneMemoryRaw);
    m_pPlaneMemoryRaw = NULL;
  }

}
