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

  /**
   * Loading the raw image of given filename into a bitmap. Dimensions
   * of the image have to be set in constructor.
   *
   * NOTE: Can be called multiple times to load more images with the
   * same dimensions.
   *
   * @returns On success: true, on failure: false
   */
  bool Load(const char* pFileName);

  struct BitMap* GetBitMap();

private:
  struct BitMap* m_pBitMap;
};

#endif
