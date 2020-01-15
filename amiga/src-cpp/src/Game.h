#ifndef GAME_H
#define GAME_H

#include "animtools.h"

#include "IGameView.h"

#include "Bullet.h"
#include "Duck.h"
#include "Hunter.h"
#include "Picture.h"

#include "PointsDisplay.h"

/**
 * The main game 'Duck chase'
 *
 * @author Uwe Rosner
 * @date 01/08/2019
 */
class Game
{
public:
  Game(IGameView& gameView);
  ~Game();

  bool Run();

  const char* LastError() const;

private:

  IGameView& m_GameView;
  PointsDisplay m_PointsDisplay;

  struct GelsInfo* m_pGelsInfo;

  const char* m_pLastError;


  Picture m_PicBackground;
  Duck m_Duck;
  Hunter m_Hunter;
  Bullet m_Bullet;

  bool gameLoop();
};

#endif
