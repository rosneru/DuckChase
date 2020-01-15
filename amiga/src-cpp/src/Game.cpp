#include <clib/graphics_protos.h>
#include <clib/lowlevel_protos.h>
#include <libraries/lowlevel.h>

#include "animtools_proto.h"
#include "StopWatch.h"

#include "Game.h"


Game::Game(IGameView& gameView)
  : m_GameView(gameView),
    m_PointsDisplay(m_GameView, 1, 5, 5, 3),  // TODO Change constants
    m_pGelsInfo(NULL),
    m_pLastError(NULL),
    m_Duck(m_GameView),
    m_Hunter(m_GameView),
    m_Bullet(m_GameView, m_Hunter)
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


bool Game::Run()
{
  if(m_GameView.Open() == false)
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
  // Setting the used color table (extracted from pic wit BtoC32)
  //
  USHORT colorsBackgr[8] =
  {
    0xAAA, 0x0, 0xFFF, 0x68B, 0x5A3, 0xEB0, 0xB52, 0xF80
  };


  // Change colors to those in colorsBackgr
  LoadRGB4(m_GameView.ViewPort(), colorsBackgr, 8);

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

  if(m_Bullet.Init() == false)
  {
    // TODO CHECK: Does m_Bullet.LastError still exist outside here?
    m_pLastError = m_Bullet.LastError();
    return false;
  }

  //
  // Load and display the background image
  //
  if(m_PicBackground.LoadFromRawFile("gfx/background_hires.raw",
                                     640, 256, 3) == FALSE)
  {
    m_pLastError = "Couldn't load background image\n";
    return false;
  }

  struct RastPort* pRastPort = m_GameView.RastPort();

  BltBitMapRastPort(m_PicBackground.GetBitMap(), 0, 0, pRastPort,
                    0, 0, 640, 256, 0xC0);

  //
  // Initially render the display
  //
  m_PointsDisplay.Clear();
  m_PointsDisplay.UpdateInfo(m_GameView.ViewName());
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
  short fpsCnt = 0;
  long fps = 0;

  StopWatch stopWatch;
  stopWatch.Start();

  bool bContinue = true;
  do
  {
    //
    // Calculate and update the FPS value
    //
    double dblElapsed = stopWatch.Pick();

    //
    // Read the Joyport and update the entities
    //
    ULONG portState = ReadJoyPort(1);
    m_Duck.Update(dblElapsed, portState);
    m_Hunter.Update(dblElapsed, portState);
    m_Bullet.Update(dblElapsed, portState);

    //
    // Render the changed scenery
    //
    m_GameView.Render();

    //
    // Calculate and display the fps
    //
    if(dblElapsed >= 0)
    {
      // Sum the fps of each frame
      fps += 1000 / dblElapsed;
      fpsCnt++;
    }

    // Every 8 frames display the avg fps
    if(fpsCnt % 8 == 0)
    {
      fps = fps >> 3; // Division by 8
      m_PointsDisplay.UpdateFps(fps);

      fps = 0;
      fpsCnt = 0;
    }


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

  return true;
}
