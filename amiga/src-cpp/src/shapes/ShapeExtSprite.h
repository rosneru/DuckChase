#ifndef SHAPE_SPRITE_WHH
#define SHAPE_SPRITE_WHH

#include <graphics/view.h>
#include <intuition/screens.h>

#include "AnimSeqExtSprite.h"
#include "ShapeBase.h"
#include "GfxResourceExtSprite.h"

/**
 * Encapsulates an animatable graphics object using the sprite hardware
 * (ExtSprite) related functions of AmigaOS graphics.libary v39+.
 *
 * To use a ShapeSpriteHw, perform the following steps:
 *
 * First, \ref SetAnimSequence should be called to set an animation (can
 * also be a single image) to the sprite. 
 * 
 * Then, with \ref SetViewPort a valid ViewPort for the sprite should be 
 * set.
 *
 * See the other public metods for more.
 *
 * @author Uwe Rosner
 * @date 04/08/2019
 */
class ShapeExtSprite : public ShapeBase
{
public:
  ShapeExtSprite(struct ViewPort* pViewPort,
                 const GfxResourceExtSprite& gfxResources);

  virtual ~ShapeExtSprite();

  void SetImage(ExtSprite* pNewImage);

  /**
   * Setting the colors of given ViewPort to given colors for sprite.
   */
  void SetVPortColorsForSprite(struct ViewPort* pViewPort, 
                               ULONG* pColors);

  //
  // Implement abstract interface ShapeBase
  //
  int Left() const;
  int Top() const;
  int Right() const;
  int Bottom() const;
  void SetInvisible();
  void SetVisible();
  bool IsVisible() const;
  bool IsGone() const;

private:
  struct ViewPort* m_pViewPort;
  long m_ImageBufSize; // Buffer for each image in bytes
  struct ExtSprite* m_pEmptySprite;
  struct ExtSprite* m_pCurrentSprite;
  int m_SpriteNumberGot; // Allocated sprite number

  void move(int x, int y);
};

#endif
