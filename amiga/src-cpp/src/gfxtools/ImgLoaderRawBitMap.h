#ifndef IMG_LOADER_RAW_BITMAP_H
#define IMG_LOADER_RAW_BITMAP_H

#include "ImgLoaderBase.h"

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
