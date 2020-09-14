#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include <graphics/gfx.h>

#include <stdio.h>

#include "BitMapTools.h"

BitMapTools::BitMapTools(const BitMap* pBitmap)
  : m_pBitmap(pBitmap),
    m_pMaskBitMap(NULL),
    m_PixelWidth(0),
    m_LineHeight(0)
{
  if(pBitmap == NULL)
  {
    throw "BitMapTools: Missing parameter.";
  }

  m_PixelWidth = GetBitMapAttr(pBitmap, BMA_WIDTH);
  m_LineHeight = GetBitMapAttr(pBitmap, BMA_HEIGHT);

  m_pMaskBitMap = AllocBitMap(m_PixelWidth, m_LineHeight, 1, BMF_CLEAR, NULL);
  if(m_pMaskBitMap == NULL)
  {
    throw "BitMapTools: Failed to allocate mask BitMap.";
  }


  // Create mask of input BitMap using the blitter in Cookie-cut mode
  BltBitMap(pBitmap, 
            0, 0,
            m_pMaskBitMap, 
            0, 0,
            m_PixelWidth, m_LineHeight, 
            0x60,                       // Minterm for cookie-cut
            0xff,                       // All planes
            NULL);
}


BitMapTools::~BitMapTools()
{
  if(m_pBitmap != NULL)
  {
    FreeBitMap(m_pMaskBitMap);
    m_pMaskBitMap = NULL;
  }
}


Rect BitMapTools::FindImageBoundingBox(const Rect& searchArea)
{
  // for(size_t i = 0; i < searchArea.Width() / 2; i ++)
  // {

  // }
  PrintMaskShell();

  return Rect();
}


void BitMapTools::PrintBitMapShell()
{
  printBitMap(m_pBitmap);
}


void BitMapTools::PrintMaskShell()
{
  printBitMap(m_pMaskBitMap);
}


void BitMapTools::printBitMap(const BitMap* pBitMap)
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
void BitMapTools::printBits(size_t const size, void const * const ptr)
{
  unsigned char *b = (unsigned char*) ptr;
  unsigned char byte;
  int i, j;

  for (i=size-1;i>=0;i--)
  {
    for (j=7;j>=0;j--)
    {
      byte = (b[i] >> j) & 1;
      printf("%u", byte);
    }
  }
}
