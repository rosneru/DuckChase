#ifndef POINTS_DISPLAY_H
#define POINTS_DISPLAY_H

#include <graphics/gfx.h>

#include "AnimSeqBitmap.h"
#include "GameColors.h"
#include "GameVars.h"
#include "GameViewBase.h"


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
  InfoDisplay(GameViewBase& gameView, 
              const GameColors& gameColors,
              GameVars& gameVars);

  ~InfoDisplay();

  void UpdateFps(size_t fps);
  void UpdateArrows();

  /**
   * Updates the strain value
   */
  void UpdateStrain(size_t newStrain, bool isForSecondBuffer);

private:
  GameViewBase& m_View;
  GameVars& m_GameVars;

  size_t m_FormerStrain;
  ULONG* m_StrainSpreadColors;

  AnimSeqBitmap m_ArrowImages;

  //RawBitMap m_ArrowDisabledImg;
  //RawBitMap m_ArrowEnabledImg;

  LONG m_Right;
  LONG m_Bottom;
};

#endif
