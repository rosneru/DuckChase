#ifndef IMG_LOADER_BASE_H
#define IMG_LOADER_BASE_H

#include <exec/types.h>


/**
 * Encapsulates a generic image loader.
 *
 * @author Uwe Rosner
 * @date 21/01/2020
 */
class PictureBase
{
public:
  virtual ULONG Width() = 0;
  virtual ULONG WordWidth() = 0;
  virtual ULONG Height() = 0;
  virtual ULONG Depth() = 0;

protected:
  PictureBase();
  virtual ~PictureBase();
};

#endif
