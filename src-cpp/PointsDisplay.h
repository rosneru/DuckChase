#ifndef POINTS_DISPLAY_H
#define POINTS_DISPLAY_H

#include <graphics/gfx.h>

#include "IGameView.h"
/**
 * A bottom 'state bar' for displaying game strikes and fps information
 *
 * To initialize this display the Clear() method should be called after
 * the view is opened.
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
                short fpsPen,
                short infoPen);

  ~PointsDisplay();

  /**
   * Clears the whole display
   */
  void Clear();


  void UpdateStrikes(short strikes);
  void UpdateFps(short fps);
  void UpdateInfo(const char* pInfo);

private:
  IGameView& m_GameView;
  short m_ViewWidth;
  short m_ViewHeight;
  short m_BackPen;
  short m_StrikesPen;
  short m_FpsPen;
  short m_InfoPen;
};

#endif
