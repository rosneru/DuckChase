#ifndef BITMAP_TOOLS_H
#define BITMAP_TOOLS_H

#include <graphics/gfx.h>
#include "Rect.h"

/**
 * Tools and functions to be performed with a picture and its mask.
 *
 * NOTE: Some of the operations will change the BitMap and BitMap mask
 * given in the constructor.
 *
 * @author Uwe Rosner
 * @date 14/09/2020
 */
class PictureProcessing
{
public:
  PictureProcessing(BitMap* pPicture, BitMap* pMask);
  virtual ~PictureProcessing();

  /**
   * Returns the bounding box of the real image pixels inside the given
   * search area in the BitMap. 
   *
   * All empty (color 0) pixels to the left, top, right and bottom in
   * the search area are skipped until a non-zero pixel is found in each
   * direction.
   */
  Rect FindBoundingBox(const Rect& searchArea);

  void PrintBitMapShell();
  void PrintMaskShell();

private:
  struct BitMap* m_pPicture;
  struct BitMap* m_pMask;
  ULONG m_PixelWidth;
  ULONG m_LineHeight;

  void printBitMap(const BitMap* pBitMap);
  void printBits(size_t const size, void const * const ptr);

  UBYTE* createRectPixelArray(const Rect& searchArea);
  UBYTE* freeRectPixelArray(const Rect& searchArea);
};


#endif
