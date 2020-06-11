#ifndef ANIM_SEQ_BASE_H
#define ANIM_SEQ_BASE_H

#include <exec/types.h>
#include <graphics/gfx.h>

/**
 * Encapsulates a generic anim sequence for shapes.
 *
 * @author Uwe Rosner
 * @date 19/01/2020
 */
class AnimSeqBase
{
public:
  int Width() const;
  int WordWidth() const;
  int Height() const;
  int Depth() const;

  size_t NumFrames() const;

  /**
   * Returns the shadow mask of the indexed image
   */
  const UBYTE* ShadowMask(size_t index) const;

protected:
  int m_Width;
  int m_WordWidth;
  int m_Height;
  int m_Depth;

  size_t m_NumFrames;

  UBYTE** m_ppShadowMasks;

  AnimSeqBase(size_t numFrames);
  virtual ~AnimSeqBase();

  UBYTE* createShadowMask(const struct BitMap* pImage);
};

#endif
