#ifndef SHADOW_MASK_H
#define SHADOW_MASK_H

#ifndef BOOST_TEST
  #include <graphics/gfx.h>
  #include <clib/exec_protos.h>
  #include <exec/types.h>
#else
  #include "amiga_types.h"
#endif

#include "Rect.h"

/**
 * Represent the shadow / collision mask of a BitMap picture.
 *
 * @author Uwe Rosner
 * @date 11/06/2020
 */
class ShadowMask
{
public:
  /**
   * Creates a mask from given bitmap. Bitmap can be deleted after
   * ShadowMask is created.
   */
  ShadowMask(const struct BitMap* pImage);

  /**
   * Uses a given mask of given size. Given mask must be valid for the
   * life time of the created ShadowMask.
   */
  ShadowMask(UBYTE* pMask, ULONG width, ULONG height);
  virtual ~ShadowMask();

  /**
   * Returns true if non-zero pixels of this mask are overlapping
   * (colliding) with non-zero pixels of the other mask. 
   *
   * Only the overlapping area is checked which are the given relative 
   * to the object rectangles.
   */
  bool IsCollision(const ShadowMask* pOther, 
                   const Rect& thisRect,
                   const Rect& otherRect) const;
  
  /**
   * Print-out the mask into a shell window. Useful for debugging
   * small masks dimensions.
   */
  void Print();

private:
  UBYTE* m_pMask;
  bool m_IsForeignMask;

  bool* m_pRowPixels;


  ULONG m_Width;
  ULONG m_WordWidth;
  ULONG m_Height;

  void calculateRowPixels(const Rect& rect, size_t row) const;
  void printBits(size_t const size, void const * const ptr);
};

#endif
