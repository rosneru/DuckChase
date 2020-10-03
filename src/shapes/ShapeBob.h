#ifndef SHAPE_BOB_H
#define SHAPE_BOB_H

#include <intuition/screens.h>

#include "AnimSeqGels.h"
#include "ShapeBase.h"
#include "GfxResourceGELs.h"

/**
 * Encapsulates an animatable graphics object using the AmigaOS /
 * graphics.libary GELs API. Creates BlitterObjects (BOBs).
 *
 * Currently it is using the animtools structures and functions as
 * presented in the RKRM libraries.
 *
 * 
 * @author Uwe Rosner
 * @date 11/07/2019
 */
class ShapeBob : public ShapeBase
{
public:
  ShapeBob(struct RastPort* pRastPort, 
           int rasterDepth,
           const GfxResourceGELs& gfxResources);

  virtual ~ShapeBob();

  void SetImage(WORD* pNewImage);

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
  struct RastPort* m_pRastPort;
  int m_RasterDepth;
  struct Bob* m_pBob;
  bool m_bIsVisible;
  LONG m_RasWidth;
  LONG m_RasHeight;
  LONG m_LineSize;
  LONG m_PlaneSize;
  
  void move(int x, int y);
};

#endif
