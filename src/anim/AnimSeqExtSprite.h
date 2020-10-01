#ifndef ANIM_SEQ_EXT_SPRITE_H
#define ANIM_SEQ_EXT_SPRITE_H

#include <graphics/sprite.h>
#include <stdlib.h>
#include "AnimSeqBase.h"
#include "BitMapPictureBase.h"

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

  /**
   * Creates a copy of the color table of given picture. Allocates
   * memory which after using must be freed with FreeVec(). The color
   * data can be loaded with LoadRgb32().
   *
   * @returns On success: the address of the the color table copy, 
   *          on error: NULL.
   */
  ULONG* deepCopyColors(const BitMapPictureBase& srcPicture);
};

#endif
