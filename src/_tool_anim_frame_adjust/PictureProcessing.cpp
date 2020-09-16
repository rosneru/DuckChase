#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include <graphics/gfx.h>

#include <stdio.h>

#include "ChunkyPixelArray.h"
#include "PictureProcessing.h"

PictureProcessing::PictureProcessing(BitMap* pPicture)
  : m_pBitMap(pPicture)
{
  if(pPicture == NULL)
  {
    throw "PictureProcessing: Missing parameter.";
  }
}


PictureProcessing::~PictureProcessing()
{

}


Rect PictureProcessing::FindBoundingBox(const Rect& searchArea)
{
  printf("searchArea: Left=%d, Top=%d, Right=%d, Bottom=%d\n\n", searchArea.Left(), searchArea.Right(), searchArea.Top(), searchArea.Bottom());

  ChunkyPixelArray pixelArray(searchArea, m_pBitMap);
  pixelArray.Print();

  Rect box = pixelArray.FindBoundingBox();

  return box;
}


void PictureProcessing::Print()
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
void PictureProcessing::printBits(size_t const size, void const* const ptr)
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
