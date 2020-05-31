#ifndef ANIM_SEQ_GELS_BOB_H
#define ANIM_SEQ_GELS_BOB_H

#include <stdlib.h>

#include "AnimSeqGels.h"

/**
 * Holds an anim sequence of ImgageData images as needed for a Bob in
 * GELs animation system. On creation the supplied ilbm picture is
 * loaded and split horizontally in as many frames as requested.
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
