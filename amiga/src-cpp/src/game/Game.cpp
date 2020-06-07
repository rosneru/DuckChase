#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/lowlevel_protos.h>
#include <libraries/lowlevel.h>

#include "Level01.h"
#include "Game.h"

Game::Game(GameViewBase& gameView, const GameWorld& gameWorld)
  : m_GameView(gameView),
    m_GameWorld(gameWorld)
{

}

void Game::Run()
{
  SetJoyPortAttrs(1,
                  SJA_Type, SJA_TYPE_AUTOSENSE,
                  TAG_END);

  SystemControl(SCON_TakeOverSys, TRUE,
                TAG_END);


  // AABoing: set task priority to 30 so that beam-synchronized stuff
  // will happen reliably. It is NOT SAFE to call intuition with this
  // high task priority.
  UWORD oldTaskPriority = 65535;
  oldTaskPriority = SetTaskPri(FindTask(0), 30);

  bool bContinue = false;
  do
  {
    // m_GameView.SwitchToFirstBuf();

    // Level01* pLevel01 = new Level01(m_GameView, m_GameWorld);
    // pLevel01->Run();
    
    // m_GameView.SwitchToFirstBuf();
    // delete pLevel01;

    bContinue = askContinue();
  } 
  while (bContinue);
  

  if(oldTaskPriority != 65535)
  {
    SetTaskPri(FindTask(0), oldTaskPriority);
  }

  SystemControl(SCON_TakeOverSys, FALSE,
                TAG_END);
}


bool Game::askContinue()
{
  clearInfoLine();
  do
  {
    WaitTOF();


    ULONG key = GetKey();
    if ((key & 0x00ff) == 0x45) // RAW code ESC key
    {
      return false;
    }

    ULONG portState = ReadJoyPort(1);
    if(portState & JPF_BTN2)
    {
      return true;
    }

  } 
  while (true);

}

void Game::clearInfoLine()
{
  int numScreenBuffers = 2;

  int rectObjectWidth = 18;
  int rectObjectHeight = 8;

  int lineWidth = 6;  // Number of vertical lines of the moving rect
  int rectWidth = (numScreenBuffers * lineWidth);
  
  int viewLeftBorder = 2;
  int viewRightBorder = m_GameView.Width() - 2 - rectWidth;
  
  int rectTop = 245;
  int rectBottom = rectTop + rectObjectHeight;

  
  for (int left = viewRightBorder; left >= viewLeftBorder; left -= lineWidth)
  {
    SetAPen(m_GameView.RastPort(), 14);
    RectFill(m_GameView.RastPort(), 
             left, 
             rectTop, 
             left + rectWidth, 
             rectBottom);

    if(left + rectObjectWidth + rectWidth < viewRightBorder)
    {
      SetAPen(m_GameView.RastPort(), 0);
      RectFill(m_GameView.RastPort(), 
               left + rectObjectWidth + 1, 
               rectTop, 
               left + rectObjectWidth + 1 + rectWidth, 
               rectBottom);
    }

    m_GameView.Render();
  }

  for (int left = viewLeftBorder; left < viewRightBorder; left += lineWidth)
  {
    SetAPen(m_GameView.RastPort(), 14);
    RectFill(m_GameView.RastPort(), 
             left, 
             rectTop, 
             left + rectWidth, 
             rectBottom);

    if(left - 3 - rectObjectWidth > viewLeftBorder)
    {
      SetAPen(m_GameView.RastPort(), 0);
      RectFill(m_GameView.RastPort(), 
               left - rectObjectWidth - 1, 
               rectTop, 
               left - rectObjectWidth - 1 + rectWidth, 
               rectBottom);
    }

    m_GameView.Render();
  }
}
