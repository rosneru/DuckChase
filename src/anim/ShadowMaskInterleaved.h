#ifndef SHADOW_MASK_INTERLEAVED_H
#define SHADOW_MASK_INTERLEAVED_H

#include <graphics/gfx.h>
#include <exec/types.h>

#include "Rect.h"

/**
 * Represent the *interleaved* shadow mask of a BitMap picture.
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

  virtual ~ShadowMaskInterleaved();

  /**
   * Print-out the mask into a shell window. Useful for debugging
   * small masks dimensions.
   */
  void Print();

private:
  UBYTE* m_pMask;
  ULONG m_MaskSizeBytes;

  bool* m_pRowPixels;


  ULONG m_Width;
  ULONG m_WordWidth;
  ULONG m_Height;
  ULONG m_Depth;

  void printBits(size_t const size, void const * const ptr);
};

#endif
