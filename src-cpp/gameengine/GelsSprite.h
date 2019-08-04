#ifndef GELS_SPRITE_H
#define GELS_SPRITE_H

#include <intuition/screens.h>
#include "animtools.h"

#define MAX_IMAGES (8)
/**
 * Encapsulates a VSprite of the AmigaOS GELs system. This class is
 * using the functions of graphics.libary v39+
 *
 * A sprite can contain up to MAX_IMAGES images which all must have the
 * same width, height and depth.
 *
 * The images can be loaded from RAW files.
 *
 * @author Uwe Rosner
 * @date 04/08/2019
 */
class GelsSprite
{
public:
  GelsSprite(int p_ImageWidth,
          int p_ImageHeight);

  ~GelsSprite();

  /**
   * Loads an image from a RAW file with given path and stores it as
   * next (or first if none already exists) image for this sprite.
   *
   * In total up to MAX_IMAGES images can be loaded for each sprite.
   *
   * @returns
   * On success true; when image can't be loaded because of wrong path,
   * not enough chip memory or MAX_IMAGES exceeded for this sprite, it
   * returns false.
   */
  bool LoadImgFromRawFile(const char* p_pPath);

  /**
   * Getting the struct Sprite* of this sprite. The sprite will be newly created
   * if it doesn't already exists.
   *
   * @returns
   * The struct sprite or null if creation has failed.
   */
  struct ExtSprite* Get();

  /**
   * Sets the next image in the sprite. If the last loaded image is
   * exceeded, the first image is set again.
   */
  void NextImage();

private:
  int m_ImageWidth;
  int m_ImageHeight;
  int m_CurrentImageIndex;

  long m_ImageBufSize;               // Buffer for each image in bytes
  struct ExtSprite* m_pSpriteDataArray[MAX_IMAGES]; // Array of pointers to the images
  struct ExtSprite* m_pCurrentSprite;
  int m_NumberOfHwSprite;

  int getNextFreeSpriteImageIdx();
  void clear();
};

#endif
