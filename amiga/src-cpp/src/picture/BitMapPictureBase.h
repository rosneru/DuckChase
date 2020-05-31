#ifndef BITMAP_PICTURE_BASE_H
#define BITMAP_PICTURE_BASE_H

#include <exec/types.h>
#include <graphics/gfx.h>

#include "PictureBase.h"

/**
 * Encapsulates a BitMapPicture
 * @author Uwe Rosner
 * @date 19/04/2020
 */
class BitMapPictureBase : public PictureBase
{
public:
  long Width();
  long WordWidth();
  long Height();
  long Depth();
  ULONG* GetColors32();

  struct BitMap* GetBitMap();

  int GetBitMapPixelColorNum(long row, long column);

protected:
  BitMapPictureBase();
  virtual ~BitMapPictureBase();
  
  struct BitMap* m_pBitMap;
  ULONG* m_pColors32;
  
};

#endif
