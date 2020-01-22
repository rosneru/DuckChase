#ifndef ANIM_SEQ_SPRITE_DATA_H
#define ANIM_SEQ_SPRITE_DATA_H

#include <graphics/sprite.h>
#include <stdlib.h>
#include "AnimSeqBase.h"

class ImgLoaderRawPure;

/**
 * Anim sequence for Sprite shapes. Uses \see ImgLoaderRawBitMap to load
 * the anim images. The methods \see GetFirstImage and \see GetNextImage
 * allow to control the animation. They return the according image which
 * can be  shown by the Sprite shape.
 *
 * @author Uwe Rosner
 * @date 19/01/2020
 */
class AnimSeqSprite : public AnimSeqBase
{
public: 
  AnimSeqSprite(int width, int height, int depth);
  virtual ~AnimSeqSprite();
  bool Load(const char** ppFileNames);

  struct ExtSprite* GetFirstImage();
  struct ExtSprite* GetNextImage();

private:
  struct ExtSprite** m_ppImages;
  size_t m_ImageCount;
  size_t m_CurrentImageId;
};
#endif
