#ifndef ANIM_SEQ_BITMAP_H
#define ANIM_SEQ_BITMAP_H

#include <stdlib.h>
#include <graphics/gfx.h>
#include "AnimSeqBase.h"


/**
 * Holds an anim sequence of BitMap images. On creation the supplied
 * ilbm picture is loaded and split horizontally in as many frames as
 * requested.
 *
 * @author Uwe Rosner
 * @date 01/06/2020
 */
class AnimSeqBitmap : public AnimSeqBase
{
public: 
  /**
   * Load an anim strip from an ilbm image. The image file is split into
   * numFrames horizontally to create the anim strip.
   */
  AnimSeqBitmap(const char* pFileName, size_t numFrames);
  virtual ~AnimSeqBitmap();

  BitMap* operator[](size_t index) const;

private:
  struct BitMap** m_ppFrames;
};

#endif
