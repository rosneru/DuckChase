#ifndef IMG_LOADER_RAW_PURE_H
#define IMG_LOADER_RAW_PURE_H

#include <dos/dos.h>

#include "PictureBase.h"

/**
 * Class for loading a RAW image file into an ImagData array. This is
 * the format which can be displayed directly as a bob.
 *
 *
 * @author Uwe Rosner
 * @date 21/01/2020
 */
class ImageDataPicture : public PictureBase
{
public: 
  /**
   * Loading the raw image of given filename into an array of WORDs.
   * Dimensions of the image have to be set in constructor.
   */
  ImageDataPicture(const char* pFileName, 
                   ULONG width, 
                   ULONG height, 
                   ULONG depth);

  /**
   * Creating raw image from a given BitMap, starting with xStart in src
   * BitMap and the width as set in constructor.
   */
  ImageDataPicture(struct BitMap* pSrcBitmap, 
                   ULONG xStart, 
                   ULONG numFrames);

  ImageDataPicture(WORD* pExternalImageData);


  virtual ~ImageDataPicture();


  WORD* GetImageData();
  long Width();
  long WordWidth();
  long Height();
  long Depth();

private:
  BPTR m_FileHandle;
  WORD* m_pImageData;
  WORD* m_pExternalImageData;

  ULONG m_Width;
  ULONG m_WordWidth;
  ULONG m_Height;
  ULONG m_Depth;
  
};

#endif
