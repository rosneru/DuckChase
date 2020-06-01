#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include "BitmapPictureBase.h"


BitmapPictureBase::BitmapPictureBase()
  : m_pBitMap(NULL),
    m_pColors32(NULL)
{
}

BitmapPictureBase::~BitmapPictureBase()
{
  if(m_pBitMap != NULL)
  {
    FreeBitMap(m_pBitMap);
    m_pBitMap = NULL;
  }

  if(m_pColors32 != NULL)
  {
    FreeVec(m_pColors32);
    m_pColors32 = NULL;
  }
}


struct BitMap* BitmapPictureBase::GetBitMap()
{
  return m_pBitMap;
}


long BitmapPictureBase::Width()
{
  if(m_pBitMap == 0)
  {
    return 0;
  }

  return GetBitMapAttr(m_pBitMap, BMA_WIDTH);
}

long BitmapPictureBase::WordWidth()
{
  if(m_pBitMap == 0)
  {
    return 0;
  }

  return ((Width() + 15) & -16) >> 4;
}

long BitmapPictureBase::Height()
{
  if(m_pBitMap == 0)
  {
    return 0;
  }

  return GetBitMapAttr(m_pBitMap, BMA_HEIGHT);
}

long BitmapPictureBase::Depth()
{
  if(m_pBitMap == 0)
  {
    return 0;
  }

  return GetBitMapAttr(m_pBitMap, BMA_DEPTH);
}



ULONG* BitmapPictureBase::GetColors32()
{
  return m_pColors32;
}


// TODO: Experimental. Probably not working for Interleaved or RTG BitMaps.
int BitmapPictureBase::GetBitMapPixelColorNum(long row, long column)
{
  if(m_pBitMap == NULL)
  {
    return -1;
  }

  if(row > m_pBitMap->Rows)
  {
    return -1;
  }

  // Calculate the index in each BitPlane array which depends on given
  // row and column
  ULONG bitId = row * m_pBitMap->BytesPerRow * 8 + column;
  ULONG byteId = bitId >> 3;
  ULONG bitInByte = 7 - (bitId - 8 * byteId);

  int colorNum = 0;
  int powerOfTwo = 1;  // Start bipotency
  for(int i = 0; i < m_pBitMap->Depth; i++)
  {
    PLANEPTR pPlaneData = m_pBitMap->Planes[i];
    UBYTE byteValue = pPlaneData[byteId];
    UBYTE bitValue = (byteValue >> bitInByte) & 0x01;
    colorNum += powerOfTwo * bitValue;

    powerOfTwo = powerOfTwo << 1;
  }

  return colorNum;
}
