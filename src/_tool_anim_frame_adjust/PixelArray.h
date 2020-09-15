#ifndef PIXEL_ARRAY_H
#define PIXEL_ARRAY_H

#include <graphics/gfx.h>
#include <graphics/rastport.h>

#include "Rect.h"

/**
 * Represensts an array of pixels and allows to perform some useful
 * operations on it.
 *
 * @author Uwe Rosner
 * @date 15/09/2020
 */
class PixelArray
{
public:
  /**
   * Creates a PixelArray of a given rectangular region in the given
   * BitMap.
   */
  PixelArray(const Rect& rect, struct BitMap* pBitmap);

  virtual ~PixelArray();
  
  void Print();

private:
  const Rect& m_Rect;
  struct RastPort m_RastPort;
  struct RastPort m_TempRastPort;
  UBYTE* m_pArray;
  size_t m_BytesPerRow;

  long findXStart();
  long findXStop();
  long findYStart();
  long findYStop();
};

#endif
