#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include <graphics/gfx.h>

#include <stdio.h>

#include "PixelArray.h"
#include "PictureProcessing.h"

PictureProcessing::PictureProcessing(BitMap* pBitMap, BitMap* pMaskBitMap)
  : m_pBitMap(pMaskBitMap),
    m_pMaskBitMap(NULL),
    m_PixelWidth(0),
    m_LineHeight(0)
{
  if(pBitMap == NULL || pMaskBitMap == NULL)
  {
    throw "PictureProcessing: Missing parameter.";
  }
}


PictureProcessing::~PictureProcessing()
{

}


Rect PictureProcessing::FindBoundingBox(const Rect& searchArea)
{
  // for(size_t i = 0; i < searchArea.Width() / 2; i ++)
  // {

  // }
  PrintMaskShell();

  PixelArray pixelArray(searchArea, m_pBitMap);
  pixelArray.Print();

  return Rect();
}


void PictureProcessing::PrintBitMapShell()
{
  printBitMap(m_pBitMap);
}


void PictureProcessing::PrintMaskShell()
{
  printf("Printing mask..\n");
  printBitMap(m_pMaskBitMap);
  printf("\n\n");
}


void PictureProcessing::printBitMap(const BitMap* pBitMap)
{
  //
  // Print the BitMap bitwise. Uncomment if needed.
  //
  for(int i = 0; i < pBitMap->Depth; i++)
  {
    PLANEPTR pPlane = pBitMap->Planes[i];
    for(int row = 0; row < pBitMap->Rows; row++)
    {
      for(int byte = 0; byte < pBitMap->BytesPerRow; byte++)
      {
        size_t offset = byte + (pBitMap->BytesPerRow * row);
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
