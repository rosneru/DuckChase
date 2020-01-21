#ifndef IMG_LOADER_RAW_PURE_H
#define IMG_LOADER_RAW_PURE_H

#include "ImgLoaderBase.h"

class ImgLoaderRawPure : public ImgLoaderBase
{
public: 
  ImgLoaderRawPure(int width, int height, int depth);
  virtual ~ImgLoaderRawPure();
  bool Load(const char* pFileName);

  const WORD* GetImageData();

private:
  WORD* m_pImageData;
};

#endif
