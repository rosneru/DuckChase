#include <clib/exec_protos.h>
#include <clib/lowlevel_protos.h>
#include <libraries/lowlevel.h>

// #include "BrownBarrel.h"
#include "IlbmBitmap.h"
#include "StopWatch.h"

#include "Game.h"


Game::Game(GameViewBase& gameView, 
           const GameWorld& gameWorld)
  : m_GameView(gameView),
    m_InfoDisplay(gameView, 
                  m_GameColors, 
                  m_MaxArrows, 
                  m_MaxStrain),
    m_IsArrowLaunching(false),
    m_IsArrowLaunchDone(false),
    m_LastFps(16),
    m_MaxStrain(118),
    m_Strain(0),
    m_StrainMustUpdateSecondBuffer(false),
    m_MaxArrows(3),
    m_NumArrowsLeft(m_MaxArrows),
    m_ArrowsMustUpdateSecondBuffer(false),
    m_Arrow(m_GameView, gameWorld, m_ArrowRes, m_Strain, true),
    m_Duck(m_GameView, gameWorld, m_DuckRes),
    m_Hunter(gameView, 
              gameWorld,
              m_HunterRes,
              m_Arrow,
              m_IsArrowLaunching, 
              m_IsArrowLaunchDone)
{
  m_Arrow.Deactivate();

  // Initialize the info display with the available arrows
  // (on both screen buffers)
  m_InfoDisplay.UpdateArrows(m_NumArrowsLeft);
  m_GameView.Render();
  m_InfoDisplay.UpdateArrows(m_NumArrowsLeft);
}


Game::~Game()
{

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

    m_Arrow.Update(elapsed_ms, portState);
    m_Duck.Update(elapsed_ms, portState);
    m_Hunter.Update(elapsed_ms, portState);

    //
    // Check if hunter is currently arming an arrow and update the info
    // display accordingly
    //
    if(m_IsArrowLaunching)
    {
      m_Strain += 4;
      m_InfoDisplay.UpdateStrain(m_Strain, false);

      // Strain must be updated in 2nd screen buffer after switched
      m_StrainMustUpdateSecondBuffer = true;
    }
    else if(m_IsArrowLaunchDone)
    {
      m_NumArrowsLeft--;
      m_InfoDisplay.UpdateArrows(m_NumArrowsLeft);

      // Arrows must be updated in 2nd screen buffer after switched
      m_ArrowsMustUpdateSecondBuffer = true;

      m_Strain = 0;
      m_InfoDisplay.UpdateStrain(m_Strain, false);

      // Strain must be updated in 2nd screen buffer after switched
      m_StrainMustUpdateSecondBuffer = true;

      m_IsArrowLaunchDone = false;
    }

    //
    // Render the entities and switch screen buffers
    //
    m_GameView.Render();

    //
    // Update changed strain and/or arrows on InfoDisplay for 2nd screen
    // buffer too (if necessary)
    //
    if(m_StrainMustUpdateSecondBuffer)
    {
      m_InfoDisplay.UpdateStrain(m_Strain, true);
      m_StrainMustUpdateSecondBuffer = false;
    }

    if(m_ArrowsMustUpdateSecondBuffer)
    {
      m_InfoDisplay.UpdateArrows(m_NumArrowsLeft);
      m_ArrowsMustUpdateSecondBuffer = false;
    }

    //
    // Check if exit key ESC has been pressed
    //
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
