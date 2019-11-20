#ifndef HW_SPRITE_H
#define HW_SPRITE_H

#include <intuition/screens.h>
#include "animtools.h"
#include "EntityBase.h"

#define MAX_IMAGES (8)
/**
 * Encapsulates an Hardware/ExtSprite provided by functions of AmigaOS
 * graphics.libary v39+
 *
 * A sprite can contain up to MAX_IMAGES images which all must have the
 * same width, height and depth.
 *
 * The images can be loaded from RAW files.
 *
 * @author Uwe Rosner
 * @date 04/08/2019
 */
class HwSprite : public EntityBase
{
public:
  HwSprite(int p_ImageWidth,
           int p_ImageHeight);

  virtual ~HwSprite();


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
  bool AddRawImage(const char* p_pPath);

  /**
   * Getting the struct Sprite* of this sprite. The sprite will be newly created
   * if it doesn't already exists.
   *
   * @returns
   * The struct sprite or null if creation has failed.
   */
  struct ExtSprite* Get();

  int SpriteNumber();

  /**
   * Sprites need a ViewPort to be displayed
   */
  void SetViewPort(struct ViewPort* pViewPort);


  //
  // Implementing part of the interface IEntity. Init(), Update(),
  // XSpeed_pps() and YSpeed_pps() must be implemented in derived
  // classes.
  //

  int XPos() const;
  int YPos() const;
  int Width() const;
  int Height() const;
  void Move(int x, int y);

  void SetInvisible();
  void SetVisible();
  bool IsVisible() const;

  void NextImage();

private:
  int m_ImageWidth;
  int m_ImageHeight;
  int m_CurrentImageIndex;

  struct ViewPort* m_pViewPort;

  long m_ImageBufSize;               // Buffer for each image in bytes
  struct ExtSprite* m_pSpriteDataArray[MAX_IMAGES]; // Array of pointers to the images
  struct ExtSprite* m_pCurrentSprite;
  struct ExtSprite* m_pEmptySprite;
  int m_HwSpriteNumber;

  int getNextFreeSpriteImageIdx();
  void clear();
};

#endif
