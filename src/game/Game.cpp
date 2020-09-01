#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/lowlevel_protos.h>
#include <libraries/lowlevel.h>

#include "Level01.h"
#include "Game.h"

Game::Game(GameViewBase& gameView, const GameWorld& gameWorld)
  : m_GameView(gameView),
    m_GameWorld(gameWorld),
    m_InfoDisplay(gameView, 
                  m_GameColors, 
                  m_GameVars)
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

  // Render again to be on the other double buf (avoids flickering)
  m_GameView.Render();

  bool bContinue = false;
  do
  {
    Level01* pLevel01 = new Level01(m_GameView, 
                                    m_InfoDisplay, 
                                    m_GameVars, 
                                    m_GameWorld, 
                                    m_StopWatch);
    pLevel01->Run();
    delete pLevel01;

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

  int numLines = 3;  // Number of vertical lines of the moving rect
  int linesWidth = (numScreenBuffers * numLines);
  
  int viewLeftBorder = 2;
  int viewRightBorder = m_GameView.Width() - 2;
  
  int rectTop = 245;
  int rectBottom = rectTop + rectObjectHeight;
  
  for (int left = viewRightBorder - linesWidth; left >= viewLeftBorder; left -= numLines)
  {
    int rightPaint = left + rectObjectWidth;
    if(rightPaint > viewRightBorder)
    {
      rightPaint = viewRightBorder;
    }

    SetAPen(m_GameView.RastPort(), 14);
    RectFill(m_GameView.RastPort(), 
             left, 
             rectTop, 
             rightPaint, 
             rectBottom);

    int rightClear = rightPaint + rectObjectWidth;
    if(rightClear > viewRightBorder)
    {
      rightClear = viewRightBorder;
    }

    if((left + rectObjectWidth) < viewRightBorder)
    {
      // printf("left + rectObjectWidth = %d, rightClear = %d\n", left + rectObjectWidth, rightClear);
      // return;
      SetAPen(m_GameView.RastPort(), 0);
      RectFill(m_GameView.RastPort(), 
               left + rectObjectWidth, 
               rectTop, 
               rightClear, 
               rectBottom);
    }

    m_GameView.Render();
  }

  SetAPen(m_GameView.RastPort(), 0);
  for (int left = (viewLeftBorder + rectObjectWidth - numLines); left >= viewLeftBorder; left -= numLines)
  {
    RectFill(m_GameView.RastPort(), 
              left, 
              rectTop, 
              left + rectObjectWidth, 
              rectBottom);
    m_GameView.Render();
  }
}
