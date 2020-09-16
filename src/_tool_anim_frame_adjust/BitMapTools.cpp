#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include <graphics/gfx.h>

#include <stdio.h>

#include "ChunkyPixelArray.h"
#include "BitMapTools.h"

BitMapTools::BitMapTools(BitMap* pPicture)
  : m_pBitMap(pPicture),
    m_RastPort()
{
  if(pPicture == NULL)
  {
    throw "BitMapTools: Missing parameter.";
  }

  // Prepare a RastPort for scrolling the BitMap
  InitRastPort(&m_RastPort);
  m_RastPort.BitMap = m_pBitMap;
  SetBPen(&m_RastPort, 0);
}


BitMapTools::~BitMapTools()
{

}


int BitMapTools::MoveObjectLeft(const Rect& searchArea, 
                                      size_t numPixels)
{
  // Create an array of junky pixels of the area
  ChunkyPixelArray chunkyPixelArray(searchArea, m_pBitMap);

  // Get the rect of the actual object (non-zero-pixels)
  Rect objectBounds = chunkyPixelArray.FindBoundingBox();

  // Limit the number of pixels to move if the object is too near to the
  // left border.
  if(objectBounds.Left() < numPixels)
  {
    numPixels = objectBounds.Left();
  }

  if(numPixels == 0)
  {
    return 0;
  }

  Rect scrollArea(searchArea.Left() + objectBounds.Left(),
                  searchArea.Top() + objectBounds.Top(),
                  searchArea.Left() + objectBounds.Right(),
                  searchArea.Top() + objectBounds.Bottom());

  ScrollRaster(&m_RastPort, numPixels, 0,
               scrollArea.Left(),
               scrollArea.Top(),
               scrollArea.Right(),
               scrollArea.Bottom());

  return numPixels;
}


int BitMapTools::MoveObjectRight(const Rect& searchArea, size_t numPixels)
{
  // Create an array of junky pixels of the area
  ChunkyPixelArray chunkyPixelArray(searchArea, m_pBitMap);

  // Get the rect of the actual object (non-zero-pixels)
  Rect objectBounds = chunkyPixelArray.FindBoundingBox();

  // Limit the number of pixels to move if the object is too near to the
  // left border.
  if(numPixels > (searchArea.Width() - objectBounds.Right()))
  {
    numPixels = searchArea.Width() - objectBounds.Right();
  }

  Rect scrollArea(searchArea.Left() + objectBounds.Left(),
                  searchArea.Top() + objectBounds.Top(),
                  searchArea.Left() + objectBounds.Right(),
                  searchArea.Top() + objectBounds.Bottom());

  ScrollRaster(&m_RastPort, -numPixels, 0,
               scrollArea.Left(),
               scrollArea.Top(),
               scrollArea.Right(),
               scrollArea.Bottom());

  return numPixels;
}


int BitMapTools::MoveObjectUp(const Rect& searchArea, size_t numPixels)
{
  // Create an array of junky pixels of the area
  ChunkyPixelArray chunkyPixelArray(searchArea, m_pBitMap);

  // Get the rect of the actual object (non-zero-pixels)
  Rect objectBounds = chunkyPixelArray.FindBoundingBox();

  // Limit the number of pixels to move if the object is too near to the
  // left border.
  if(objectBounds.Top() < numPixels)
  {
    numPixels = objectBounds.Top();
  }

  if(numPixels == 0)
  {
    return 0;
  }

  Rect scrollArea(searchArea.Left() + objectBounds.Left(),
                  searchArea.Top() + objectBounds.Top(),
                  searchArea.Left() + objectBounds.Right(),
                  searchArea.Top() + objectBounds.Bottom());

  ScrollRaster(&m_RastPort, 0, numPixels,
               scrollArea.Left(),
               scrollArea.Top(),
               scrollArea.Right(),
               scrollArea.Bottom());

  return numPixels;
}


int BitMapTools::MoveObjectDown(const Rect& searchArea, size_t numPixels)
{
  // Create an array of junky pixels of the area
  ChunkyPixelArray chunkyPixelArray(searchArea, m_pBitMap);

  // Get the rect of the actual object (non-zero-pixels)
  Rect objectBounds = chunkyPixelArray.FindBoundingBox();

  // Limit the number of pixels to move if the object is too near to the
  // left border.
  if(numPixels > (searchArea.Height() - objectBounds.Bottom()))
  {
    numPixels = searchArea.Height() - objectBounds.Bottom();
  }

  Rect scrollArea(searchArea.Left() + objectBounds.Left(),
                  searchArea.Top() + objectBounds.Top(),
                  searchArea.Left() + objectBounds.Right(),
                  searchArea.Top() + objectBounds.Bottom());

  ScrollRaster(&m_RastPort, 0, -numPixels,
               scrollArea.Left(),
               scrollArea.Top(),
               scrollArea.Right(),
               scrollArea.Bottom());

  return numPixels;
}


void BitMapTools::Print()
{
  for(int i = 0; i < m_pBitMap->Depth; i++)
  {
    PLANEPTR pPlane = m_pBitMap->Planes[i];
    for(int row = 0; row < m_pBitMap->Rows; row++)
    {
      for(int byte = 0; byte < m_pBitMap->BytesPerRow; byte++)
      {
        size_t offset = byte + (m_pBitMap->BytesPerRow * row);
        printBits(1, pPlane + offset);
        printf(" ");
      }
      printf("\n");
    }
    printf("\n");
  }
}

// assumes little endian
void BitMapTools::printBits(size_t const size, void const* const ptr)
{
  unsigned char* p = (unsigned char*)ptr;
  unsigned char byte;
  int i, j;

  for (i = size - 1; i >= 0; i--)
  {
    for (j = 7; j >= 0; j--)
    {
      byte = (p[i] >> j) & 1;
      printf("%u", byte);
    }
  }
}
