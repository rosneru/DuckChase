#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include <graphics/gfx.h>

#include <stdio.h>

#include "ShadowMaskInterleaved.h"

ShadowMaskInterleaved::ShadowMaskInterleaved(struct BitMap* pImage)
  : m_pMask(NULL),
    m_IsForeignMask(false)  // The mask is created here. It is *not* foreign.
{
  struct BitMap *pMaskBitMap, *pTempBitMap;
  struct RastPort rp, temprp;
  UBYTE *pPixelArray, *pY, *pX, *pSrcRow, *pDstRow;
  ULONG x, y, ySrc, yDst, numMaskCopies;
  ULONG bytesPerRow, arrayBytes;
  long transparentPen = 0;

  if(pImage == NULL)
  {
    throw "ShadowMaskInterleaved: No BitMap image given.";
  }

  m_Width = GetBitMapAttr(pImage, BMA_WIDTH);
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
  m_Height = GetBitMapAttr(pImage, BMA_HEIGHT);
  m_Depth = GetBitMapAttr(pImage, BMA_DEPTH);
  ULONG planeSize = RASSIZE(m_Width, m_Height);

  // Default: Source BitMap is planar, only one copy of the mask needed
  numMaskCopies = 1;

  // But check if source BitMap is interleaved
  if(GetBitMapAttr(pImage, BMA_FLAGS) & BMF_INTERLEAVED)
  {
    // Source BitMap is interleaved; must copy mask m_Depth times
    numMaskCopies = m_Depth;
  }

  // Allocate memory for the final mask (m_Depth * m_Height because interleaved)
  m_pMask = (UBYTE*)AllocVec(planeSize * numMaskCopies, MEMF_CHIP|MEMF_CLEAR);
  if(m_pMask == NULL)
  {
    throw "ShadowMaskInterleaved: Failed to allocate memory for frame mask.";
  }

  // Allocate temporary Bitmap to write the mask
  pMaskBitMap = AllocBitMap (m_Width, m_Height, 1, BMF_CLEAR, NULL);
  if(pMaskBitMap == NULL)
  {
    FreeRaster(m_pMask, m_Width, m_Depth * m_Height);
    throw "ShadowMaskInterleaved: Failed to allocate temporary mask BitMap.";
  }


  pTempBitMap = AllocBitMap (m_Width, m_Height, m_Depth, BMF_CLEAR, NULL);
  if(pTempBitMap == NULL)
  {
    FreeBitMap(pMaskBitMap);
    FreeRaster(m_pMask, m_Width, m_Depth * m_Height);
    throw "ShadowMaskInterleaved: Failed to allocate temporary BitMap.";
  }

  // See autodocs for Read/WritePixelArray8
  arrayBytes = ((((m_Width + 15) >> 4) << 4) * (m_Height + 1));
  bytesPerRow = arrayBytes / m_Height;

  // Allocate memory for PixelArray
  pPixelArray = (UBYTE*)AllocVec (arrayBytes, MEMF_PUBLIC);
  if(pPixelArray == NULL)
  {
    FreeBitMap(pTempBitMap);
    FreeBitMap(pMaskBitMap);
    FreeRaster(m_pMask, m_Width, m_Depth * m_Height);
    throw "ShadowMaskInterleaved: Failed to allocate memory for pixel array.";
  }

  InitRastPort (&rp);
  rp.BitMap = pImage;  // Point the RastPort to source BitMap to create
                        // the PixelArray from it

  // as defined in autodocs for Read/WritePixelArray8
  temprp = rp;
  temprp.Layer = NULL;
  temprp.BitMap = pTempBitMap;

  ReadPixelArray8 (&rp, 0, 0, m_Width - 1, m_Height - 1, pPixelArray, &temprp);

  pY = pPixelArray;
  for (y = 0; y < m_Height; y++)
  {
    pX = pY;

    for (x = 0; x < m_Width; x++)
    {
      // set transparent pixels to 0, others to 1
      if(*pX == transparentPen)
      {
        *pX = 0;
      }
      else
      {
        *pX = 1;
      }

      pX++;
    }

    pY += bytesPerRow;
  }

  // Set destination for WritePixelArray8 to mask BitMap
  rp.BitMap = pMaskBitMap;

  // Write the mask
	WritePixelArray8 (&rp, 0, 0, m_Width - 1, m_Height - 1, pPixelArray, &temprp);

  // Now use the 'real' BytesPerRow from the planar / non-interleaved
  // mask BitMap
  bytesPerRow = pMaskBitMap->BytesPerRow;

  for(ySrc = 0; ySrc < m_Height; ySrc++)
  {
    pSrcRow = pMaskBitMap->Planes[0] + (ySrc * bytesPerRow);
    for(yDst = ySrc * bytesPerRow; yDst < (ySrc * bytesPerRow) + numMaskCopies; yDst++)
    {
      pDstRow = m_pMask + (yDst * bytesPerRow);
      CopyMem(pSrcRow, pDstRow, bytesPerRow);
    }
  }

  FreeVec(pPixelArray);
  FreeBitMap(pTempBitMap);
  FreeBitMap(pMaskBitMap);
}

ShadowMaskInterleaved::ShadowMaskInterleaved(UBYTE* m_pMask, 
                       ULONG m_Width, 
                       ULONG m_Height)
  : m_pMask(m_pMask),
    m_IsForeignMask(true),  // The mask has been passed from outside. It *is* foreign.
    m_Width(m_Width),
    m_WordWidth(((m_Width + 15) & -16) >> 4),
    m_Height(m_Height)
{
  m_pRowPixels = new bool[m_WordWidth * 2 * 8];
}

ShadowMaskInterleaved::~ShadowMaskInterleaved()
{
  delete[] m_pRowPixels;

  if((!m_IsForeignMask) && (m_pMask != NULL))
  {
    FreeVec(m_pMask);
    m_pMask = NULL;
  }
}


void ShadowMaskInterleaved::Print()
{
  ULONG bytesPerRow = m_WordWidth * 2;
  PLANEPTR pPlane = m_pMask;
  for(size_t row = 0; row < m_Height; row++)
  {
    for(int byte = 0; byte < bytesPerRow; byte++)
    {
      size_t offset = byte + (bytesPerRow * row);
      printBits(1, pPlane + offset);
      printf(" ");
    }
    printf("\n");
  }
  printf("\n");
}


void ShadowMaskInterleaved::calculateRowPixels(const Rect& rect, size_t row) const
{
  for(int column = rect.Left(); column <= rect.Right(); column++)
  {
    size_t rowByte = column >> 3;
    size_t byteId = row * (m_WordWidth * 2) + rowByte;
    size_t bitInByte = 7 - (column - rowByte * 8);
    UBYTE byteValue = m_pMask[byteId];
    size_t bitValue = byteValue & (1 << bitInByte);

    if(bitValue != 0)
    {
      m_pRowPixels[column - rect.Left()] = true;
    }
    else
    {
      m_pRowPixels[column - rect.Left()] = false;
    }
  }
}

// assumes little endian
void ShadowMaskInterleaved::printBits(size_t const size, void const* const ptr)
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