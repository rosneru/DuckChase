#ifndef CHUNKY_PIXEL_ARRAY_H
#define CHUNKY_PIXEL_ARRAY_H

#include <graphics/gfx.h>
#include <graphics/rastport.h>

#include "Rect.h"

/**
 * A chunky pixel representation of a rectangular area of a planar
 * BitMap picture.
 *
 * @author Uwe Rosner
 * @date 15/09/2020
 */
class ChunkyPixelArray
{
public:
  /**
   * Creates a PixelArray of a given rectangular region in the given
   * BitMap.
   */
  ChunkyPixelArray(const Rect& rect, struct BitMap* pPicture);

  /**
   * Freeing all allocated resources.
   */
  virtual ~ChunkyPixelArray();

  /**
   * Return the bounding box of the non-zero pixels in this array.
   */
  Rect FindBoundingBox();
  
  /**
   * Print-out the array into a shell window. Useful for debugging
   * with small array dimensions.
   */
  void Print();

private:
  const Rect& m_Rect;
  struct RastPort m_RastPort;
  struct RastPort m_TempRastPort;
  UBYTE* m_pArray;


  /**
   * Scans all columns from left to right and returns the index of the
   * first column that contains a non-zero pixel.
   * 
   * Returns 0 when no non-zero pixel found in any column.
   */
  long findXStart();

  /**
   * Scans all columns from right to left and returns the index of the
   * last column that contains a non-zero pixel.
   * 
   * Returns 0 when no non-zero pixel found in any column.
   */
  long findXStop();

  /**
   * Scans all rows from top to bottom and returns the first index
   * that contains a non-zero pixel.
   * 
   * Returns 0 when no non-zero pixel found in any row.
   */
  long findYStart();

  /**
   * Scans all rows from bottom to topand returns the last index
   * that contains a non-zero pixel.
   * 
   * Returns 0 when no non-zero pixel found in any row.
   */
  long findYStop();
};

#endif
