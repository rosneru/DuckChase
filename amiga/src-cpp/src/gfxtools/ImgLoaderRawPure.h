#ifndef IMG_LOADER_RAW_PURE_H
#define IMG_LOADER_RAW_PURE_H

#include "ImgLoaderBase.h"

/**
 * Class for loading a RAW image file into an ImagData array. This is
 * the format which can be displayed directly as a bob.
 *
 *
 * @author Uwe Rosner
 * @date 21/01/2020
 */
class ImgLoaderRawPure : public ImgLoaderBase
{
public: 
  ImgLoaderRawPure(int width, int height, int depth);
  virtual ~ImgLoaderRawPure();

  /**
   * Loading the raw image of given filename into an array of WORDs.
   * Dimensions of the image have to be set in constructor.
   *
   * NOTE: Can be called multiple times to load more images with the
   * same dimensions.
   *
   * @returns On success: true, on failure: false
   */
  bool Load(const char* pFileName);

  WORD* GetImageData();

private:
  WORD* m_pImageData;
};

#endif
