#include <clib/exec_protos.h>
#include <clib/lowlevel_protos.h>
#include <libraries/lowlevel.h>

// #include "BrownBarrel.h"
#include "IlbmBitMap.h"
#include "StopWatch.h"

#include "Game.h"

#define MAX_ARROWS 3


Game::Game(GameViewBase& gameView, 
           const GameWorld& gameWorld)
  : m_GameView(gameView),
    m_InfoDisplay(gameView, 
                  m_GameColors, 
                  m_MaxArrows, 
                  m_MaxStrain),
    m_LastFps(16),
    m_MaxStrain(118),
    m_MaxArrows(5),
    m_NumArrowsLeft(m_MaxArrows),
    m_Duck(m_GameView, gameWorld, m_DuckRes),
    m_Hunter(gameView, 
              gameWorld,
              m_HunterRes,
              m_IsArrowLaunching, 
              m_IsArrowLaunchDone),
    m_IsArrowLaunching(false),
    m_IsArrowLaunchDone(false)
{

  // Create a collection of some arrows
  for(int i = 0; i < MAX_ARROWS; i++)
  {
    // First arrow (sprite) should steel mouse pointer;
    // NOTE: This should not be here, find a better place.
    bool steelMouse = (i == 0);

    m_Arrows.Push(new Arrow(m_GameView, gameWorld, m_ArrowRes, steelMouse));
  }
}


Game::~Game()
{
  size_t numArrows = m_Arrows.Size();
  for(size_t i = 0; i < numArrows; i++)
  {
    Arrow* pArrow = (Arrow*)m_Arrows.Pop();
    delete pArrow;
    pArrow = NULL;
  }
}

void Game::DisableDoubleBuf()
{
//   m_GameView.DisableDoubleBuf();

//   // TODO: Find a better way, hold a list for entities
//   m_Jumpman.DisableDoubleBuf();
}


void Game::Run()
{
  StopWatch stopWatch;
  stopWatch.Start();

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

  short frameCounter = -2;
  long elapsed_ms = 0;
  long elapsedSinceLastBarrel_ms = 40;
  bool justDrawFps = false;
  bool bContinue = true;
  do
  {
    //
    // Calculate the elapsed time and update the FPS value
    //
    frameCounter++;
    if(frameCounter > 16)
    {
      frameCounter = 1;
      elapsed_ms = stopWatch.Pick(false) >> 4;
      elapsedSinceLastBarrel_ms += elapsed_ms;
      if(elapsed_ms > 0)
      {
        // Sum the fps of each frame
        m_LastFps = (short)(1000 / elapsed_ms);
        m_InfoDisplay.UpdateFps(m_LastFps);

        // To also appear in the double buffer, ensure that fps is
        // updated in next frame too.
        justDrawFps = true;
      }
    }
    else if(justDrawFps)
    {
      m_InfoDisplay.UpdateFps(m_LastFps);
      justDrawFps = false;
    }

    //
    // Read the Joyport and update the entities
    //
    ULONG portState = ReadJoyPort(1);

    m_Hunter.Update(elapsed_ms, portState);
    m_Duck.Update(elapsed_ms, portState);

    for(size_t i = 0; i < m_Arrows.Size(); i++)
    {
      Arrow* pArrow = (Arrow*)m_Arrows[i];
      if(pArrow->IsAlive())
      {
        pArrow->Update(elapsed_ms, portState);
      }
    }

    // if(elapsedSinceLastBarrel_ms >= 40)
    // {
    //   for(size_t i = 0; i < m_BrownBarrells.Size(); i++)
    //   {
    //     BrownBarrel* pBrownBarrel = (BrownBarrel*)m_BrownBarrells[i];

    //     if(!pBrownBarrel->IsAlive())
    //     {
    //       elapsedSinceLastBarrel_ms = 0;
    //       pBrownBarrel->Activate(20, 10, 80, 80);
    //       break;
    //     }
    //   }
    // }

    // Render the changed scenery
    m_GameView.Render();

    // Check if exit key ESC has been pressed
    ULONG key = GetKey();
    if((key & 0xff) == 0x45)
    {
      // ESC pressed, quit the game
      bContinue = false;
    }
  }
  while (bContinue);

  if(oldTaskPriority != 65535)
  {
    SetTaskPri(FindTask(0), oldTaskPriority);
  }

  SystemControl(SCON_TakeOverSys, FALSE,
                TAG_END);
}
