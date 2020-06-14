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
  virtual long Width() const = 0;
  virtual long WordWidth() const = 0;
  virtual long Height() const = 0;
  virtual long Depth() const = 0;

protected:
  PictureBase();
  virtual ~PictureBase();
};

#endif
