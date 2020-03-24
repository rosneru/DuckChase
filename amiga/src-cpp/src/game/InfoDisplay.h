#ifndef POINTS_DISPLAY_H
#define POINTS_DISPLAY_H

#include <graphics/gfx.h>

#include "GameColors.h"
#include "ImgLoaderRawBitMap.h"
#include "IGameView.h"

/**
 * A bottom 'state bar' for displaying game the number of arrows the 
 * player has left and fps information
 *
 * @author Uwe Rosner
 * @date 01/08/2019
 */
class InfoDisplay
{
public:
  InfoDisplay(IGameView& gameView, 
              GameColors& gameColors,
              const size_t& maxArrows,
              const size_t& maxStrain);

  ~InfoDisplay();

  void UpdateFps(size_t fps);
  void UpdateArrows(size_t arrowsLeft);
  void UpdateStrain(size_t newStrain);

private:
  IGameView& m_View;
  const size_t& m_MaxArrows;
  const size_t& m_MaxStrain;
  size_t m_FormerStrain;
  ULONG* m_StrainSpreadColors;

  ImgLoaderRawBitMap m_ArrowDisabledImg;
  ImgLoaderRawBitMap m_ArrowEnabledImg;

  LONG m_Left;
  LONG m_Right;
  LONG m_Top;
  LONG m_Bottom;
};

#endif