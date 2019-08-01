#ifndef POINTS_DISPLAY_H
#define POINTS_DISPLAY_H

#include <graphics/gfx.h>

#include "IGameView.h"
/**
 * A bottom 'state bar' for displaying game strikes and fps information
 *
 * @author Uwe Rosner
 * @date 01/08/2019
 */
class PointsDisplay
{
public:
  PointsDisplay(IGameView& gameView,
                short backPen, 
                short strikesPen,
                short fpsPen);

  ~PointsDisplay();


  void UpdateStrikes(short strikes);
  void UpdateFps(short fps);

private:
  IGameView& m_GameView;
  struct RastPort* m_pRastPort;
  short m_ViewWidth;
  short m_ViewHeight;
  short m_BackPen;
  short m_StrikesPen;
  short m_FpsPen;
};

#endif
