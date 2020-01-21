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
 * A bob can contain up to MAX_IMAGES images which all must have the
 * same width, height and depth.
 *
 * The images can be loaded from a memory array, RAW file or by datatype
 * methods (TBD).
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
   * Bobs are onyl visible in one deicated RastPort..
   */
  void AddToRastPort(struct RastPort* pRastPort);


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
  int m_RasterDepth;

  struct Bob* m_pBob;
  AnimSeqBob* m_pAnimSeq;

  struct RastPort* m_pRastPort;

  bool m_bIsVisible;

  void createBob();
};

#endif
