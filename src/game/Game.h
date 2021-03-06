#ifndef GAME_H
#define GAME_H

#include "GameViewBase.h"
#include "GameColors.h"
#include "GameVars.h"
#include "GameWorld.h"
#include "InfoDisplay.h"
#include "StopWatch.h"

/**
 * The game itself. Wraps together the separate levels.
 *
 * @author Uwe Rosner
 * @date 07/06/2019
 */
class Game
{
public:
  Game(GameViewBase& gameView, const GameWorld& gameWorld);

  void Run();

private:
  GameViewBase& m_GameView;
  const GameWorld& m_GameWorld;
  GameVars m_GameVars;
  InfoDisplay m_InfoDisplay;
  StopWatch m_StopWatch;
  const GameColors m_GameColors;

  bool askContinue();
  void clearInfoLine();
};

#endif
