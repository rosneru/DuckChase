#ifndef SHADOW_MASK_H
#define SHADOW_MASK_H

#include <graphics/gfx.h>

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
  ShadowMask(const struct BitMap* pImage);
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
private:
  UBYTE* m_pMask;

};

#endif
