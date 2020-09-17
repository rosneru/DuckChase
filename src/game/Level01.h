#ifndef LEVEL_01_H
#define LEVEL_01_H


#include "GameViewBase.h"
#include "GameVars.h"
#include "GameWorld.h"
#include "GelsLayer.h"
#include "OpenIlbmPictureBitMap.h"
#include "EntityCollection.h"

#include "Arrow.h"
#include "ArrowGfxResources.h"
#include "Duck.h"
#include "DuckGfxResources.h"
#include "Hunter.h"
#include "HunterGfxResources.h"
#include "StopWatch.h"

#include "GameColors.h"
#include "InfoDisplay.h"

/**
 * The first level of 'Duck chase'
 *
 * @author Uwe Rosner
 * @date 01/08/2019
 */
class Level01
{
public:
  Level01(GameViewBase& gameView, 
          InfoDisplay& infoDisplay,
          GameVars& gameVars,
          const GameWorld& gameWorld,
          StopWatch& stopWatch);

  virtual ~Level01();

  /**
   * When this is called before invoking \see Run, the \see GameViewBase 
   * and the \see ShapeBob are set to only use single buffering.
   * 
   * Intended for testing purposes only.
   */
  void DisableDoubleBuf();

  void Run();

private:
  GameViewBase& m_GameView;
  InfoDisplay& m_InfoDisplay;
  GameVars& m_GameVars;
  StopWatch& m_StopWatch;

  OpenIlbmPictureBitMap m_BackgroundPicture;
  
  bool m_IsArrowFlightPrepared;
  bool m_IsArrowFlightFinished;
  bool m_IsStrike;
  size_t m_LastFps;
  
  size_t m_Strain;

  // EntityCollection m_Arrows;
  DuckGfxResources m_DuckRes;
  Duck m_Duck;
  ArrowGfxResources m_ArrowRes;
  Arrow m_Arrow;
  HunterGfxResources m_HunterRes;
  Hunter m_Hunter;

  void gameLoop();
  void displayWinner();
};

#endif
