#ifndef IMG_LOADER_BASE_H
#define IMG_LOADER_BASE_H


/**
 * Encapsulates a generic image loader.
 *
 * @author Uwe Rosner
 * @date 21/01/2020
 */
class PictureBase
{
public:
  virtual long Width() = 0;
  virtual long WordWidth() = 0;
  virtual long Height() = 0;
  virtual long Depth() = 0;

protected:
  PictureBase();
  virtual ~PictureBase();
};

#endif
