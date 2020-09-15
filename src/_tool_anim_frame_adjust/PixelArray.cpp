#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>
#include <exec/memory.h>

#include <stdio.h>

#include "PixelArray.h"

PixelArray::PixelArray(const Rect& rect, struct BitMap* pBitmap)
  : m_Rect(rect),
    m_RastPort(),
    m_TempRastPort(),
    m_pArray(NULL),
    m_BytesPerRow(0)
{
  m_RastPort.BitMap = NULL;

  if(pBitmap == NULL)
  {
    throw "PixelArray: Missing Parameter.";
  }

  // TODO: Maybe check if rect is fit into BitMap and else throw an
  // exception.

  int depth = GetBitMapAttr(pBitmap, BMA_DEPTH);
  if(depth > 8)
  {
    throw "PixelArray: Not more than 8 bitplanes supported.";
  }

  // Main RastPort as needed by Read/WritePixelArray8()
  InitRastPort(&m_RastPort);
  m_RastPort.BitMap = pBitmap;

  // Temporaray RastPort as needed by Read/WritePixelArray8()
  m_TempRastPort = m_RastPort;
  m_TempRastPort.Layer = NULL;
  m_TempRastPort.BitMap = AllocBitMap(rect.Width(), 
                                      rect.Height(), 
                                      1, 
                                      BMF_CLEAR, 
                                      NULL);

  if(m_TempRastPort.BitMap == NULL)
  {
    throw "PixelArray: Failed to allocate a BitMap.";
  }

  // TODO What happens if rect.width is not a factor of 16?
  m_BytesPerRow = ((rect.Width() + 15) >> 4) << 4;
  m_pArray = (UBYTE*)AllocVec(m_BytesPerRow * rect.Height(), MEMF_PUBLIC);

  // Converting the rectangular area of the bitmap into an array
  size_t count = ReadPixelArray8(&m_RastPort, 
                               rect.Left(),
                               rect.Top(),
                               rect.Right(),
                               rect.Bottom(),
                               m_pArray,
                               &m_TempRastPort);

  if(count != m_Rect.Area())
  {
    throw "PixelArray: Wrong number of Pixels read.";
  }
}


PixelArray::~PixelArray()
{
  if(m_pArray != NULL)
  {
    FreeVec(m_pArray);
    m_pArray = NULL;
  }

  if(m_TempRastPort.BitMap != NULL)
  {
    FreeBitMap(m_TempRastPort.BitMap);
    m_TempRastPort.BitMap = NULL;
  }
}


Rect PixelArray::FindBoundingBox()
{
  long left = findXStart();
  long top = findXStop();
  long right = findYStart();
  long bottom = findXStop();

  printf("Bounding box:");
  printf("left = %ld, top = %ld, right = %ld, bottom = %ld\n\n", left, top, right, bottom);
  
  
  return Rect(left, top, right, bottom);
}


void PixelArray::Print()
{
  printf("Printing pixel array..\n");

  for(size_t y = 0; y < m_Rect.Height(); y++)
  {
    for(size_t x = 0; x < m_Rect.Width(); x++)
    {
      size_t idx = x + y * m_Rect.Height();
      printf("%u", m_pArray[idx]);
    }
  }

  printf("\n\n");
}


long PixelArray::findXStart()
{
  for(size_t x = 0; x < m_Rect.Width(); x++)
  {
    for(size_t y = 0; y < m_Rect.Height(); y++)
    {
      size_t idx = x + y * m_Rect.Height();
      if(m_pArray[idx] != 0)
      {
        return x;
      }
    }
  }

  return 0;
}


long PixelArray::findXStop()
{
  for(size_t x = m_Rect.Width() - 1; x >= 0 ; x--)
  {
    for(size_t y = 0; y < m_Rect.Height(); y++)
    {
      size_t idx = x + y * m_Rect.Height();
      if(m_pArray[idx] != 0)
      {
        return x;
      }
    }
  }

  return 0;
}


long PixelArray::findYStart()
{
  for(size_t y = 0; y < m_Rect.Height(); y++)
  {
    for(size_t x = 0; x < m_Rect.Width(); x++)
    {
      size_t idx = x + y * m_Rect.Height();
      if(m_pArray[idx] != 0)
      {
        return y;
      }
    }
  }

  return 0;
}


long PixelArray::findYStop()
{
  for(size_t y = m_Rect.Height() - 1; y >= 0 ; y--)
  {
    for(size_t x = 0; x < m_Rect.Width(); x++)
    {
      size_t idx = x + y * m_Rect.Height();
      if(m_pArray[idx] != 0)
      {
        return y;
      }
    }
  }

  return 0;
}
