#ifndef ANIM_SEQ_BASE_H
#define ANIM_SEQ_BASE_H

#include <exec/types.h>

class AnimSeqBase
{

protected:
  int m_Width;
  int m_WordWidth;
  int m_Height;
  int m_Depth;

  AnimSeqBase(int width, int height, int depth);
  virtual ~AnimSeqBase();

  WORD* loadRawImageData(const char* pPath);
};

#endif