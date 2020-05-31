#ifndef ANIM_SEQ_EXT_SPRITE_H
#define ANIM_SEQ_EXT_SPRITE_H

#include <graphics/sprite.h>
#include <stdlib.h>
#include "AnimSeqBase.h"

class ImgLoaderImageData;

/**
 * Holds an anim sequence of ExtSprite images. On creation the supplied
 * ilbm picture is loaded and split horizontally in as many frames as
 * requested.
 *
 * @author Uwe Rosner
 * @date 19/01/2020
 */
class AnimSeqExtSprite : public AnimSeqBase
{
public: 
  /**
   * Load an anim strip from an ilbm image. The image file is split into
   * numFrames horizontally to create the anim strip.
   */
  AnimSeqExtSprite(const char* pFileName, size_t numFrames);
  virtual ~AnimSeqExtSprite();

  ExtSprite* operator[](size_t index) const;

  ULONG* GetColors32() const;


private:
  struct BitMap* m_pFrameBitMap;
  struct ExtSprite** m_ppFrames;
  ULONG* m_pColors32;
};

#endif
