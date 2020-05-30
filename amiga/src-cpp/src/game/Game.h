#ifndef GAME_H
#define GAME_H


#include "GameViewBase.h"
#include "GameWorld.h"
#include "GelsLayer.h"
#include "EntityCollection.h"

#include "BrownBarrelResources.h"
#include "Jumpman.h"
#include "JumpmanResources.h"


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
  size_t m_LastFps;
  
  const size_t m_MaxStrain;
  const size_t m_MaxArrows;
  size_t m_NumArrowsLeft;


  GameColors m_GameColors;

  BrownBarrelResources m_BrownBarrelResources;
  EntityCollection m_BrownBarrells;
  JumpmanResources m_JumpmanResources;
  Jumpman m_Jumpman;

  bool m_IsArrowLaunching;
  bool m_IsArrowLaunchDone;
};

#endif
