#ifndef ANIM_SEQ_GELS_H
#define ANIM_SEQ_GELS_H

#include <stdlib.h>
#include <graphics/gfx.h>

#include "AnimSeqBase.h"

class OpenImageDataPicture;

/**
 * Abstract class of a anim sequence for Bobs and VSprites of the Amiga
 * GELs system. Uses \see ImageDataPicture to load the anim images. The
 * methods \see GetFirstImage and \see GetNextImage allow to control the
 * animation. They return the according image which can be shown by
 * shape.
 *
 * @author Uwe Rosner
 * @date 19/01/2020
 */
class AnimSeqGels : public AnimSeqBase
{
public: 
  AnimSeqGels(size_t numFrames);
  virtual ~AnimSeqGels();

  WORD* operator[](size_t index) const;

protected:
  struct BitMap* m_pFrameBitMap;
  OpenImageDataPicture** m_ppFrames;
};

#endif
