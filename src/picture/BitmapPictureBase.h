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
class BitmapPictureBase : public PictureBase
{
public:
  long Width() const;
  long WordWidth() const;
  long Height() const;
  long Depth() const;

  const ULONG* GetColors32() const;
  ULONG GetModeId() const;

  const struct BitMap* GetBitMap() const;

  /**
   * Returns the transparency mask for this picture. If the mask doesn't
   * exists it will be craeted on firts call of this method.
   * 
   * The mask is freed when this picture is destroyed.
   * 
   * FIXME: Isn't this already done on ilbm loading..?
   */
  const struct BitMap* GetMaskBitMap();

  int GetBitMapPixelColorNum(long row, long column) const;

protected:
  BitmapPictureBase();
  virtual ~BitmapPictureBase();
  
  struct BitMap* m_pBitMap;
  struct BitMap* m_pBitMapMask;
  ULONG* m_pColors32;
  ULONG m_ModeId;
  
};

#endif
