#ifndef ANIM_SEQ_BOB_DATA_H
#define ANIM_SEQ_BOB_DATA_H

#include <stdlib.h>

#include "AnimSeqBase.h"

class AnimSeqBob : public AnimSeqBase
{
public: 
  AnimSeqBob(int width, int height, int depth);
  virtual ~AnimSeqBob();
  bool Load(const char** ppFileNames);



private:
  WORD** m_ppImages;
  size_t m_ImageCount;
};

#endif
