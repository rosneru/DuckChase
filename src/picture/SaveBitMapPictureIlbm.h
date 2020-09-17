#ifndef SAVE_BITMAP_ILBM_PICTURE_H
#define SAVE_BITMAP_ILBM_PICTURE_H

#include <graphics/gfx.h>
#include "BitMapPictureBase.h"

/**
 * Class for saving a BitMap to an iff ilbm file using the
 * iffparse.library. Code of newiff39 from Commodore is used.
 *
 * @author Uwe Rosner
 * @date 17/09/2020
 */
class SaveBitMapPictureIlbm
{
public:
  SaveBitMapPictureIlbm(const BitMapPictureBase& picture, 
                        const char* pFileName);

  virtual ~SaveBitMapPictureIlbm();

private:
  const ULONG m_BODY_BUF_SIZE;
  ULONG m_ModeId;
  UBYTE* bodybuf;

  void cleanup();
};

#endif
