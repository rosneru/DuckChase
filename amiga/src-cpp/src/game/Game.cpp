#include <clib/exec_protos.h>
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
    m_GameView.SwitchToFirstBuf();

    Level01* pLevel01 = new Level01(m_GameView, m_GameWorld);
    pLevel01->Run();
    
    m_GameView.SwitchToFirstBuf();
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