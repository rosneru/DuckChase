#ifndef ANIM_SEQ_BOB_DATA_H
#define ANIM_SEQ_BOB_DATA_H

#include <stdlib.h>

#include "AnimSeqBase.h"

class ImgLoaderRawPure;

/**
 * Anim sequence for Bob shapes. Uses \see ImgLoaderRawPure to load
 * the anim images. The methods \see GetFirstImage and \see GetNextImage
 * allow to control the animation. They return the according image which
 * can be  shown by the Bob shape.
 *
 * @author Uwe Rosner
 * @date 19/01/2020
 */
class AnimSeqBob : public AnimSeqBase
{
public: 
  AnimSeqBob(int width, int height, int depth);
  virtual ~AnimSeqBob();
  bool Load(const char** ppFileNames);

  WORD* GetFirstImage();
  WORD* GetNextImage();

private:
  ImgLoaderRawPure** m_ppImages;
  size_t m_ImageCount;
  size_t m_CurrentImageId;
};

#endif
