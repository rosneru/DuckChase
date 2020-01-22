#ifndef IMG_LOADER_RAW_BITMAP_H
#define IMG_LOADER_RAW_BITMAP_H

#include "ImgLoaderBase.h"

/**
 * Class for loading a RAW image file into a Bitmap.
 *
 * @author Uwe Rosner
 * @date 21/01/2020
 */
class ImgLoaderRawBitMap : public ImgLoaderBase
{
public: 
  ImgLoaderRawBitMap(int width, int height, int depth);
  virtual ~ImgLoaderRawBitMap();
  bool Load(const char* pFileName);

  struct BitMap* GetBitMap();

private:
  struct BitMap* m_pBitMap;
};

#endif
