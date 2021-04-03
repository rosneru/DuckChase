#ifndef SHADOW_MASK_INTERLEAVED_H
#define SHADOW_MASK_INTERLEAVED_H

#include <graphics/gfx.h>
#include <exec/types.h>

#include "Rect.h"

/**
 * Represent the shadow / collision mask of a BitMap picture.
 *
 * @author Uwe Rosner
 * @date 03/04/2021
 */
class ShadowMaskInterleaved
{
public:
  /**
   * Creates a mask from given bitmap. Bitmap can be deleted after
   * ShadowMaskInterleaved is created.
   */
  ShadowMaskInterleaved(struct BitMap* pImage);

  /**
   * Uses a given mask of given size. Given mask must be valid for the
   * life time of the created ShadowMaskInterleaved.
   */
  ShadowMaskInterleaved(UBYTE* pMask, ULONG width, ULONG height);
  virtual ~ShadowMaskInterleaved();

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
  ULONG m_Depth;

  void calculateRowPixels(const Rect& rect, size_t row) const;
  void printBits(size_t const size, void const * const ptr);
};

#endif
