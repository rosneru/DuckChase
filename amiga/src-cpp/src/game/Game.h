#ifndef GAME_H
#define GAME_H

#include "GameViewBase.h"
#include "GameWorld.h"

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

  bool askContinue();
};

#endif
