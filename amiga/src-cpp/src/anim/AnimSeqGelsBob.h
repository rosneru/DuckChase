#ifndef ANIM_SEQ_GELS_BOB_H
#define ANIM_SEQ_GELS_BOB_H

#include <stdlib.h>

#include "AnimSeqGels.h"

/**
 * Class of a anim sequence for VSprites of the Amiga GELs system. Uses
 * \see ImageDataPicture to load the anim images. The methods \see
 * GetFirstImage and \see GetNextImage allow to control the animation.
 * They return the according image which can be shown by shape.
 *
 * @author Uwe Rosner
 * @date 19/01/2020
 */
class AnimSeqGelsBob : public AnimSeqGels
{
public: 
  AnimSeqGelsBob(const char* pFileName, size_t numFrames);
  virtual ~AnimSeqGelsBob();
};

#endif
