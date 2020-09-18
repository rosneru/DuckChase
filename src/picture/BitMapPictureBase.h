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
  long Width() const;
  long WordWidth() const;
  long Height() const;
  long Depth() const;

  ULONG* GetColors32() const;
  ULONG GetModeId() const;

  const struct BitMap* GetBitMap() const;

  /**
   * Returns the transparency mask for this picture. If the mask doesn't
   * exist already (e.g. has not already been load while ilbm picture
   * loading) it will be created on first call of this method.
   *
   * The mask is freed when this picture is destroyed.
   */
  const struct BitMap* GetMask();

  int GetBitMapPixelColorNum(long row, long column) const;

protected:
  BitMapPictureBase();
  virtual ~BitMapPictureBase();
  
  struct BitMap* m_pBitMap;
  struct BitMap* m_pBitMapMask;
  ULONG* m_pColors32;
  ULONG m_ModeId;
  
};

#endif
