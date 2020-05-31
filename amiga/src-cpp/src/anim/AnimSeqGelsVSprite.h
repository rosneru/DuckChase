#ifndef ANIM_SEQ_GELS_VSPRITE_H
#define ANIM_SEQ_GELS_VSPRITE_H

#include <stdlib.h>

#include "AnimSeqGels.h"

class ImageDataPicture;

/**
 * Holds an anim sequence of ImgageData images as needed for a VSprite
 * in GELs animation system. On creation the supplied ilbm picture is
 * loaded and split horizontally in as many frames as requested.
 * 
 * NOTE: Not tested since RAII refactoring.
 *
 * @author Uwe Rosner
 * @date 19/01/2020
 */
class AnimSeqGelsVSprite : public AnimSeqGels
{
public: 
  AnimSeqGelsVSprite(const char* pFileName, size_t numFrames);
  virtual ~AnimSeqGelsVSprite();

private:
  ImageDataPicture* m_pImgLoaderSrc;
};

#endif
