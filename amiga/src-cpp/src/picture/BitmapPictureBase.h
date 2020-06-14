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
  const ULONG* GetColors32();

  const struct BitMap* GetBitMap() const;

  /**
   * FIXME: Isn't this already done on ilbm loading..?
   */
  const struct BitMap* CreateBitMapMask();

  int GetBitMapPixelColorNum(long row, long column) const;

protected:
  BitmapPictureBase();
  virtual ~BitmapPictureBase();
  
  struct BitMap* m_pBitMap;
  struct BitMap* m_pBitMapMask;
  ULONG* m_pColors32;
  
};

#endif
