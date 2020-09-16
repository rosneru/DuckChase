#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include <graphics/gfx.h>

#include <stdio.h>

#include "ChunkyPixelArray.h"
#include "PictureProcessing.h"

PictureProcessing::PictureProcessing(BitMap* pPicture, BitMap* pMask)
  : m_pPicture(pPicture),
    m_pMask(pMask),
    m_PixelWidth(0),
    m_LineHeight(0)
{
  if(pPicture == NULL || pMask == NULL)
  {
    throw "PictureProcessing: Missing parameter.";
  }
}


PictureProcessing::~PictureProcessing()
{

}


Rect PictureProcessing::FindBoundingBox(const Rect& searchArea)
{
  PrintMaskShell();

  printf("searchArea: Left=%d, Top=%d, Right=%d, Bottom=%d\n\n", searchArea.Left(), searchArea.Right(), searchArea.Top(), searchArea.Bottom());

  ChunkyPixelArray pixelArray(searchArea, m_pPicture);
  pixelArray.Print();

  Rect box = pixelArray.FindBoundingBox();

  return box;
}


void PictureProcessing::PrintBitMapShell()
{
  printBitMap(m_pPicture);
}


void PictureProcessing::PrintMaskShell()
{
  printf("Printing mask..\n");
  printBitMap(m_pMask);
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
