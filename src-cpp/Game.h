#ifndef GAME_H
#define GAME_H

#include "animtools.h"
#include "GameViewAdvanced.h"
#include "GameViewSimple.h"
#include "GelsBob.h"
#include "GelsSprite.h"
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
  GelsBob m_BobDuck;
  GelsBob m_BobHunter;
  GelsSprite m_SpriteBullet;

  struct Bob* m_pBobDuck;
  struct Bob* m_pBobHunter;
  struct ExtSprite* m_pSpriteBullet;

  bool gameLoop();
};

#endif
