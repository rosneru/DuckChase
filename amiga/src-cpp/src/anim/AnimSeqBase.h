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
  int Width() const;
  int WordWidth() const;
  int Height() const;
  int Depth() const;

  size_t NumFrames() const;

protected:
  int m_Width;
  int m_WordWidth;
  int m_Height;
  int m_Depth;

  size_t m_NumFrames;

  AnimSeqBase();
  virtual ~AnimSeqBase();
};

#endif
