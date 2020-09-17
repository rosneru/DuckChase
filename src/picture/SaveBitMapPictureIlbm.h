#ifndef SAVE_BITMAP_ILBM_PICTURE_H
#define SAVE_BITMAP_ILBM_PICTURE_H

#include <graphics/gfx.h>

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
  SaveBitMapPictureIlbm(const struct BitMap* pBitMap, 
                        const char* pFileName);

  virtual ~SaveBitMapPictureIlbm();
};

#endif
