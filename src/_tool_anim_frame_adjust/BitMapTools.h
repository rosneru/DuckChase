#ifndef BITMAP_TOOLS_H
#define BITMAP_TOOLS_H

#include <graphics/gfx.h>
#include "Rect.h"

/**
 * Tools and functions to be performed with a struct BitMap.
 * 
 * @author Uwe Rosner
 * @date 14/09/2020
 */
class BitMapTools
{
public:
  BitMapTools(const BitMap* pBitmap);
  virtual ~BitMapTools();

  /**
   * Returns the bounding box of the real image pixels inside the given
   * search area in the BitMap. 
   *
   * All empty (color 0) pixels to the left, top, right and bottom in
   * the search area are skipped until a non-zero pixel is found in each
   * direction.
   */
  Rect FindImageBoundingBox(const Rect& searchArea);

  void PrintBitMapShell();
  void PrintMaskShell();

private:
  const struct BitMap* m_pBitmap;
  struct BitMap* m_pMaskBitMap;
  ULONG m_PixelWidth;
  ULONG m_LineHeight;

  void printBitMap(const BitMap* pBitMap);
  void printBits(size_t const size, void const * const ptr);
};


#endif
