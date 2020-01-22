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
  bool Load(const char* pFileName);

  WORD* GetImageData();

private:
  WORD* m_pImageData;
};

#endif
