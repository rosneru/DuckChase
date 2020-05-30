#ifndef ANIM_SEQ_BASE_H
#define ANIM_SEQ_BASE_H

#include <exec/types.h>


/**
 * Encapsulates a generic anim sequence for shapes.
 *
 * @author Uwe Rosner
 * @date 19/01/2020
 */
class AnimSeqBase
{
public:
  size_t Width() const;
  size_t WordWidth() const;
  size_t Height() const;
  size_t Depth() const;

  size_t NumFrames() const;

protected:
  size_t m_Width;
  size_t m_WordWidth;
  size_t m_Height;
  size_t m_Depth;

  size_t m_NumFrames;

  AnimSeqBase();
  virtual ~AnimSeqBase();
};

#endif
