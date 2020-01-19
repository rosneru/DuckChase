#ifndef ANIM_SEQ_BOB_DATA_H
#define ANIM_SEQ_BOB_DATA_H

#include "AnimSeqBase.h"

class AnimSeqBob
{
public: 
  AnimSeqBob(int width, int height, int depth);
  bool Load(const char** ppFileNames);
};

#endif
