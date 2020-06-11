#ifndef SHADOW_MASK_H
#define SHADOW_MASK_H

#include <graphics/gfx.h>

class ShadowMask
{
public:
  ShadowMask(const struct BitMap* pImage);
  virtual ~ShadowMask();

  /**
   * Returns the shadow mask of the indexed image
   */
  const UBYTE* Mask(size_t index) const;
private:
  UBYTE* m_pMask;

};

#endif
