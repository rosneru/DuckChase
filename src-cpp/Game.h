#ifndef GAME_H
#define GAME_H

#include "animtools.h"
#include "GameViewAdvanced.h"
#include "GameViewSimple.h"
#include "GelsBob.h"
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
  Game(bool bAdvancedView = false);
  ~Game();

  bool Run();

  const char* LastError() const;

private:
  GameViewSimple m_GameViewSimple;
  GameViewAdvanced m_GameViewAdvanced;
  IGameView* m_pGameView;
  PointsDisplay* m_pPointsDisplay;

  struct GelsInfo* m_pGelsInfo;

  const char* m_pLastError;


  Picture m_PicBackground;
  GelsBob m_BobDuck;
  GelsBob m_BobHunter;

  struct Bob* m_pBobDuck;
  struct Bob* m_pBobHunter;

  bool gameLoop();
};

#endif
