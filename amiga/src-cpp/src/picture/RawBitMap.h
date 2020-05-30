#ifndef RAW_BITMAP_H
#define RAW_BITMAP_H

#include <dos/dos.h>
#include <graphics/gfx.h>

#include "BitMapPictureBase.h"


/**
 * Class for loading a iff ilbm image file into a Bitmap using the 
 * iffparse.library. Also code of newiff from Commodore  is heavily 
 * used.
 *
 * @author Uwe Rosner
 * @date 04/04/2020
 */
class RawBitMap : public BitMapPictureBase
{
public: 
  /**
   * Creates a BitMap file by loading the RAW image of given filename
   * into a bitmap. Dimensions of the image have to be set in
   * constructor. 
   */
  RawBitMap(const char* pFileName, ULONG width, ULONG height, ULONG depth);

  virtual ~RawBitMap();


private:
  BPTR m_FileHandle;
};

#endif
