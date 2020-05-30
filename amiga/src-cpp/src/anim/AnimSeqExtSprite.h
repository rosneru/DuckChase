#ifndef ANIM_SEQ_EXT_SPRITE_H
#define ANIM_SEQ_EXT_SPRITE_H

#include <graphics/sprite.h>
#include <stdlib.h>
#include "AnimSeqBase.h"

class ImgLoaderImageData;

/**
 * Anim sequence of ExtSprite images. Uses \see ImgLoaderRawBitMap to
 * load the anim images. The methods \see GetFirstImage and \see
 * GetNextImage allow to control the animation. They return the
 * according image which can be  shown by the Sprite shape.
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

  ULONG* GetColors32();


private:
  struct BitMap* m_pFrameBitMap;
  struct ExtSprite** m_ppFrames;
  ULONG* m_pColors32;
};

#endif
