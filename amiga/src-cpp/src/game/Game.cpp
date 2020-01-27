#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>
#include <clib/lowlevel_protos.h>
#include <libraries/lowlevel.h>

#include "animtools_proto.h"
#include "StopWatch.h"

#include "Game.h"


Game::Game(IGameView& gameView)
  : m_GameView(gameView),
    m_InfoDisplay(gameView, m_GameColors, m_MaxArrows, m_MaxStrain),
    m_MaxStrain(118),
    m_MaxArrows(5),
    m_NumArrowsLeft(m_MaxArrows),
    m_pGelsInfo(NULL),
    m_pLastError(NULL),
    m_Duck(gameView),
    m_Hunter(gameView, m_bArrowIsArmed),
    m_Arrow(gameView, m_GameColors , m_Hunter),
    m_bArrowIsArmed(false)
{

}


Game::~Game()
{
  // Free the resources allocated by the Gels system
  if(m_pGelsInfo != NULL)
  {
    cleanupGelSys(m_pGelsInfo, m_GameView.RastPort());
  }

  WaitTOF();
  WaitTOF();

  m_GameView.Close();
}

void Game::DisableDoubleBuf()
{
  m_GameView.DisableDoubleBuf();

  // TODO: Find a better way, hold a list for entities
  m_Duck.DisableDoubleBuf();
  m_Hunter.DisableDoubleBuf();
}


bool Game::Run()
{

  if(m_GameView.Open(m_GameColors) == false)
  {
    m_pLastError = m_GameView.LastError();
    return false;
  }

  // Initialize the Gels system
  m_pGelsInfo = setupGelSys(m_GameView.RastPort(), 0x03);
  if(m_pGelsInfo == NULL)
  {
    m_pLastError = "Could not initialize the Gels system\n";
    return false;
  }

  //
  // Initializing all the entities which populate the game world
  //
  if(m_Duck.Init() == false)
  {
    // TODO CHECK: Does m_Duck.LastError still exist outside here?
    m_pLastError = m_Duck.LastError();
    return false;
  }

  if(m_Hunter.Init() == false)
  {
    // TODO CHECK: Does m_Hunter.LastError still exist outside here?
    m_pLastError = m_Hunter.LastError();
    return false;
  }

  if(m_Arrow.Init() == false)
  {
    // TODO CHECK: Does m_Bullet.LastError still exist outside here?
    m_pLastError = m_Arrow.LastError();
    return false;
  }

  //
  // Load and display the background image
  //
  if(m_BackgroundImage.LoadFromRawFile("gfx/background.raw",
                                     640, 256, 4) == FALSE)
  {
    m_pLastError = "Couldn't load background image\n";
    return false;
  }

  struct RastPort* pRastPort = m_GameView.RastPort();

  BltBitMapRastPort(m_BackgroundImage.GetBitMap(), 0, 0, pRastPort,
                    0, 0, 640, 256, 0xC0);

  //
  // Initially render the display
  //
  m_InfoDisplay.UpdateFps(0);
  m_InfoDisplay.UpdateArrows(m_NumArrowsLeft);

  // At beginning render twice to be on the right (double) buffer again
  m_GameView.Render();
  m_GameView.Render();


  return gameLoop();
}


const char* Game::LastError() const
{
  return m_pLastError;
}


bool Game::gameLoop()
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
  bool bContinue = true;
  do
  {
    //
    // Calculate the elapsed time and update the FPS value
    //
    frameCounter++;
    if(frameCounter > 50)
    {
      frameCounter = 1;
      long elapsed = stopWatch.Pick(false) / 50;
      if(elapsed >= 0)
      {
        // Sum the fps of each frame
        long fps = (short)(1000 / elapsed);
        m_InfoDisplay.UpdateFps(fps);
      }
    }
    else if(frameCounter < 1)
    {
      // This occurs one time only, namely after the first cycle. So the
      // elapsed time is calculated early and entities can be updated
      // properly before the first 50 frames are counted.
      elapsed_ms = stopWatch.Pick(false);
    }

    //
    // Read the Joyport and update the entities
    //
    ULONG portState = ReadJoyPort(1);
    m_Duck.Update(elapsed_ms, portState);
    m_Hunter.Update(elapsed_ms, portState);
    m_Arrow.Update(elapsed_ms, portState);

    //
    // Render the changed scenery
    //
    m_GameView.Render();


    //
    // Check if exit key ESC has been pressed
    //
    ULONG key = GetKey();
    if((key & 0xff) == 0x45)
    {
      // ESC pressed
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

  return true;
}
