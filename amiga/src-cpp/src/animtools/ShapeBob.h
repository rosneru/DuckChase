#ifndef SHAPE_BOB_H
#define SHAPE_BOB_H

#include <intuition/screens.h>

#include "AnimSeqBob.h"
#include "animtools.h"
#include "ShapeBase.h"

#define MAX_IMAGES 8
/**
 * Encapsulates an animatable graphics object using the blitter hardware
 * (Gels system) related functions of AmigaOS graphics.libary.
 *
 * Currently it is using the animtools structures and functions as
 * presented in the RKRM libraries.
 *
 * To use a ShapeBob, perform the followng steps:
 *
 * First, \ref SetAnimSequence should be called to set an animation (can
 * also be a single image) to the bob. 
 * 
 * Then, with \ref SetRastPort a valid RastPort for the bob should be 
 * set.
 *
 * See the other public metods for more.
 * 
 * @author Uwe Rosner
 * @date 11/07/2019
 */
class ShapeBob : public ShapeBase
{
public:
  ShapeBob(int rasterDepth);

  virtual ~ShapeBob();


  /**
   * Sets the RastPort in which the bob is drawn.
   */
  void SetRastPort(struct RastPort* pRastPort);


  //
  // Implementing the abstract methods of interface ShapeBase.
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
  int m_RasterDepth;

  struct Bob* m_pBob;
  AnimSeqBob* m_pAnimSeq;

  struct RastPort* m_pRastPort;

  bool m_bIsVisible;

  void createBob();
};

#endif
