#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>
#include <exec/memory.h>

#include <stdio.h>

#include "ChunkyPixelArray.h"

ChunkyPixelArray::ChunkyPixelArray(const Rect& rect, struct BitMap* pPicture)
  : m_Rect(rect),
    m_RastPort(),
    m_TempRastPort(),
    m_pArray(NULL)
{
  m_RastPort.BitMap = NULL;

  if(pPicture == NULL)
  {
    throw "ChunkyPixelArray: Missing Parameter.";
  }

  // TODO: Maybe check if rect is fit into BitMap and else throw an
  // exception.

  int depth = GetBitMapAttr(pPicture, BMA_DEPTH);
  if(depth > 8)
  {
    throw "ChunkyPixelArray: No more than 8 bitplanes supported.";
  }

  // Main RastPort as needed by Read/WritePixelArray8()
  InitRastPort(&m_RastPort);
  m_RastPort.BitMap = pPicture;

  // Temporaray RastPort as needed by Read/WritePixelArray8()
  m_TempRastPort = m_RastPort;
  m_TempRastPort.Layer = NULL;
  m_TempRastPort.BitMap = AllocBitMap(rect.Width(), 
                                      rect.Height(), 
                                      depth, 
                                      BMF_CLEAR, 
                                      NULL);

  if(m_TempRastPort.BitMap == NULL)
  {
    throw "ChunkyPixelArray: Failed to allocate a BitMap.";
  }

  // see description of WritePixelArray8 in autodocs.
  size_t bytes = ((((rect.Width() + 15) >> 4) << 4) * (rect.Bottom() - rect.Top() + 1));
  m_pArray = (UBYTE*)AllocVec(bytes, MEMF_PUBLIC);

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
    throw "ChunkyPixelArray: Wrong number of Pixels read.";
  }
}


ChunkyPixelArray::~ChunkyPixelArray()
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


Rect ChunkyPixelArray::FindBoundingBox()
{
  long left = findXStart();
  long top = findYStart();
  long right = findXStop();
  long bottom = findYStop();
  
  return Rect(left, top, right, bottom);
}


void ChunkyPixelArray::Print()
{
  printf("Printing chunky pixel array..\n");

  for(size_t y = 0; y < m_Rect.Height(); y++)
  {
    for(size_t x = 0; x < m_Rect.Width(); x++)
    {
      size_t idx = x + y * m_Rect.Width();
      printf("%3u", m_pArray[idx]);
    }
    printf("\n");
  }

  printf("\n\n");
}


long ChunkyPixelArray::findXStart()
{
  for(size_t x = 0; x < m_Rect.Width(); x++)
  {
    for(size_t y = 0; y < m_Rect.Height(); y++)
    {
      size_t idx = x + y * m_Rect.Width();
      if(m_pArray[idx] != 0)
      {
        return x;
      }
    }
  }

  return 0;
}


long ChunkyPixelArray::findXStop()
{
  for(size_t x = m_Rect.Width() - 1; x >= 0 ; x--)
  {
    for(size_t y = 0; y < m_Rect.Height(); y++)
    {
      size_t idx = x + y * m_Rect.Width();
      if(m_pArray[idx] != 0)
      {
        return x;
      }
    }
  }

  return 0;
}


long ChunkyPixelArray::findYStart()
{
  for(size_t y = 0; y < m_Rect.Height(); y++)
  {
    for(size_t x = 0; x < m_Rect.Width(); x++)
    {
      size_t idx = x + y * m_Rect.Width();
      if(m_pArray[idx] != 0)
      {
        return y;
      }
    }
  }

  return 0;
}


long ChunkyPixelArray::findYStop()
{
  for(size_t y = m_Rect.Height() - 1; y >= 0 ; y--)
  {
    for(size_t x = 0; x < m_Rect.Width(); x++)
    {
      size_t idx = x + y * m_Rect.Width();
      if(m_pArray[idx] != 0)
      {
        return y;
      }
    }
  }

  return 0;
}
