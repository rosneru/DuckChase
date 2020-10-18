#ifndef BITMAP_TOOLS_H
#define BITMAP_TOOLS_H

#include <graphics/gfx.h>
#include <graphics/rastport.h>

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
class BitMapTools
{
public:
  BitMapTools(BitMap* pPicture);
  virtual ~BitMapTools();

  /**
   * Move the object (the non-zero pixels) inside the given area
   * left by numPixels pixels.
   * 
   * @returns the number of pixels the object actually was moved.
   */
  int MoveObjectLeft(const Rect& searchArea, size_t numPixels);

  /**
   * Move the object (the non-zero pixels) inside the given area
   * right by numPixels pixels.
   * 
   * @returns the number of pixels the object actually was moved.
   */
  int MoveObjectRight(const Rect& searchArea, size_t numPixels);

  /**
   * Move the object (the non-zero pixels) inside the given area
   * up by numPixels pixels.
   * 
   * @returns the number of pixels the object actually was moved.
   */
  int MoveObjectUp(const Rect& searchArea, size_t numPixels);

  /**
   * Move the object (the non-zero pixels) inside the given area
   * down by numPixels pixels.
   * 
   * @returns the number of pixels the object actually was moved.
   */
  int MoveObjectDown(const Rect& searchArea, size_t numPixels);

  /**
   * Print-out the array into a shell window. Useful for debugging
   * with small array dimensions.
   */
  void Print();

private:
  struct BitMap* m_pBitMap;
  struct RastPort m_RastPort;

  void printBits(size_t const size, void const * const ptr);
};


#endif
