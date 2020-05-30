#ifndef SHAPE_SPRITE_V_H
#define SHAPE_SPRITE_V_H

#include <intuition/screens.h>

#include "AnimSeqGels.h"
#include "ShapeBase.h"
#include "ResourceGELs.h"

/**
 * Encapsulates an animatable graphics object using the AmigaOS /
 * graphics.libary GELs API. Creates VSprites.
 *
 * Currently it is using the animtools structures and functions as
 * presented in the RKRM libraries.
 *
 * @author Uwe Rosner
 * @date 29/02/2020
 */
class ShapeVSprite : public ShapeBase
{
public:
  ShapeVSprite(struct RastPort* pRastPort, 
               const ResourceGELs& gfxResources,
               WORD* pSpriteColors);

  virtual ~ShapeVSprite();

  void SetImage(WORD* pImage);

  //
  // Implement abstract interface ShapeBase
  //
  int Left() const;
  int Top() const;
  void SetInvisible();
  void SetVisible();
  bool IsVisible() const;
  bool IsGone() const;


private:
  struct RastPort* m_pRastPort;
  WORD* m_pSpriteColors;
  struct VSprite* m_pSprite;
  bool m_bIsVisible;
  LONG m_LineSize;
  LONG m_PlaneSize;

  void move(int x, int y);
};

#endif
