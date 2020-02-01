#ifndef GAME_H
#define GAME_H

#include "animtools.h"

#include "IGameView.h"

#include "Arrow.h"
#include "Duck.h"
#include "Hunter.h"
#include "Picture.h"

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
  Game(IGameView& gameView);
  ~Game();

  /**
   * When this is called before invoking \see Run, the \see IGameView 
   * and the \see ShapeBob are set to only use single buffering.
   * 
   * Intended for testing purposes only.
   */
  void DisableDoubleBuf();

  bool Run();

  const char* LastError() const;

private:
  IGameView& m_GameView;
  InfoDisplay m_InfoDisplay;
  
  const size_t m_MaxStrain;
  const size_t m_MaxArrows;
  size_t m_NumArrowsLeft;

  struct GelsInfo* m_pGelsInfo;

  const char* m_pLastError;

  GameColors m_GameColors;

  Picture m_BackgroundImage;
  Duck m_Duck;
  Hunter m_Hunter;
  Arrow m_Arrow;

  bool m_IsArrowLaunching;
  bool m_IsArrowLaunchDone;

  bool gameLoop();
};

#endif
