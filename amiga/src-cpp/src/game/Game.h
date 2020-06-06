#ifndef GAME_H
#define GAME_H


#include "GameViewBase.h"
#include "GameWorld.h"
#include "GelsLayer.h"
#include "EntityCollection.h"

#include "Arrow.h"
#include "ArrowResources.h"
#include "Duck.h"
#include "DuckResources.h"
#include "Hunter.h"
#include "HunterResources.h"


#include "GameColors.h"
#include "InfoDisplay.h"

/**
 * The main game 'Duck chase'
 *
 * @author Uwe Rosner
 * @date 01/08/2019
 */
class Game
{
public:
  Game(GameViewBase& gameView, 
       const GameWorld& gameWorld);

  virtual ~Game();

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

  InfoDisplay m_InfoDisplay;
  
  bool m_IsArrowLaunching;
  bool m_IsArrowLaunchDone;
  bool m_Strike;
  size_t m_LastFps;
  
  const size_t m_MaxStrain;
  size_t m_Strain;
  bool m_StrainMustUpdateSecondBuffer;
  const size_t m_MaxArrows;
  size_t m_NumArrowsLeft;
  bool m_ArrowsMustUpdateSecondBuffer;


  GameColors m_GameColors;


  // EntityCollection m_Arrows;
  DuckResources m_DuckRes;
  Duck m_Duck;
  ArrowResources m_ArrowRes;
  Arrow m_Arrow;
  HunterResources m_HunterRes;
  Hunter m_Hunter;

  void displayWinner();
};

#endif
