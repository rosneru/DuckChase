#ifndef SHAPE_SPRITE_H
#define SHAPE_SPRITE_H

#include <intuition/screens.h>

#include "AnimSeqSprite.h"
#include "ShapeBase.h"
#include "animtools.h"

/**
 * Encapsulates an animatable graphics object using the sprite hardware
 * (ExtSprite) related functions of AmigaOS graphics.libary v39+.
 *
 * A sprite can contain up to MAX_IMAGES images which all must have the
 * same width, height and depth.
 *
 * The images can be loaded from RAW files.
 *
 * @author Uwe Rosner
 * @date 04/08/2019
 */
class ShapeSprite : public ShapeBase
{
public:
  ShapeSprite(int p_ImageWidth, int p_ImageHeight);

  virtual ~ShapeSprite();

  /**
   * Getting the struct Sprite* of this sprite. The sprite will be newly
   * created if it doesn't already exists.
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

  void SetAnimSequence(AnimSeqBase* pAnimSequence);
  void NextImage();

private:
  int m_Width;
  int m_Height;

  AnimSeqSprite* m_pAnimSeq;

  struct ViewPort* m_pViewPort;

  long m_ImageBufSize; // Buffer for each image in bytes
  struct ExtSprite* m_pEmptySprite;
  struct ExtSprite* m_pCurrentSprite;
  int m_HardwareSpriteNumber;


  void createSprite();
};

#endif
