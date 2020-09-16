#ifndef BITMAP_TOOLS_H
#define BITMAP_TOOLS_H

#include <graphics/gfx.h>
#include "Rect.h"

/**
 * Tools and functions to be performed with a picture.
 *
 * NOTE: Some of the operations will change the BitMap given in the
 * constructor.
 *
 * @author Uwe Rosner
 * @date 14/09/2020
 */
class PictureProcessing
{
public:
  PictureProcessing(BitMap* pPicture);
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


  /**
   * Print-out the array into a shell window. Useful for debugging
   * with small array dimensions.
   */
  void Print();

private:
  struct BitMap* m_pBitMap;

  void printBits(size_t const size, void const * const ptr);

  UBYTE* createRectPixelArray(const Rect& searchArea);
  UBYTE* freeRectPixelArray(const Rect& searchArea);
};


#endif
