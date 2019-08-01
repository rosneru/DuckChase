#include <clib/graphics_protos.h>
#include <clib/lowlevel_protos.h>
#include <libraries/lowlevel.h>

#include "animtools_proto.h"
#include "StopWatch.h"

#include "Game.h"


Game::Game(bool bAdvancedView)
  : m_GameViewSimple(640, 256, 3),    // Don't panic, light-weight init
    m_GameViewAdvanced(640, 256, 3),  // Don't panic, light-weight init
    m_pGameView(&m_GameViewSimple),
    m_pGelsInfo(NULL),
    m_pLastError(NULL),
    m_BobDuck(m_pGameView->Depth, 59, 21, 3),
    m_BobHunter(m_pGameView->Depth, 16, 22, 3),
    m_pBobDuck(NULL),
    m_pBobHunter(NULL)
{
  if(bAdvancedView)
  {
    m_pGameView = &m_GameViewAdvanced;
  }
}


Game::~Game()
{
  if(m_pPointsDisplay != NULL)
  {
    delete m_pPointsDisplay;
    m_pPointsDisplay = NULL;
  }

  if(m_pBobDuck != NULL)
  {
    RemBob(m_pBobDuck);
    m_pBobDuck = NULL;
  }

    if(m_pBobHunter != NULL)
  {
    RemBob(m_pBobHunter);
    m_pBobHunter = NULL;
  }

  // Free the resources allocated by the Gels system
  if(m_pGelsInfo != NULL)
  {
    cleanupGelSys(m_pGelsInfo, m_pGameView->RastPort());
  }

  WaitTOF();
  WaitTOF();

  m_pGameView->Close();
}


bool Game::Run()
{
  if(m_pGameView->Open() == false)
  {
    m_pLastError = m_pGameView->LastError();
    return false;
  }

  // Initialize the Gels system
  m_pGelsInfo = setupGelSys(m_pGameView->RastPort(), 0x03);
  if(m_pGelsInfo == NULL)
  {
    m_pLastError = "Could not initialize the Gels system\n";
    return false;
  }

  //
  // Setting the used color table (extracted from pic wit BtoC32)
  //
  USHORT colortable[8] =
  {
    0xAAA, 0x0, 0xFFF, 0x68B, 0x5A3, 0xEB0, 0xB52, 0xF80
  };

  // Change colors to those in colortable
  LoadRGB4(m_pGameView->ViewPort(), colortable, 8);

  //
  // Loading all the Bobs images
  //
  if(m_BobDuck.LoadImgFromRawFile("/gfx/ente1_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bob duck image #1 /gfx/ente1_hires.raw";
    return false;
  }

  if(m_BobDuck.LoadImgFromRawFile("/gfx/ente2_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bob duck image #2 /gfx/ente2_hires.raw";
    return false;
  }

  if(m_BobHunter.LoadImgFromRawFile("/gfx/jaeger1_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bob hunter image #1 /gfx/jaeger1_hires.raw";
    return false;
  }

  if(m_BobHunter.LoadImgFromRawFile("/gfx/jaeger2_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bob hunter image #2 /gfx/jaeger2_hires.raw";
    return false;
  }

  m_pBobDuck = m_BobDuck.Get();
  m_pBobHunter = m_BobHunter.Get();

  if(m_pBobDuck == NULL)
  {
    m_pLastError = "Couldn't acquire bob duck\n";
    return false;
  }

  if(m_pBobHunter == NULL)
  {
    m_pLastError = "Couldn't acquire bob hunter\n";
    return false;
  }

  //
  // Load and display the background image
  //
  if(m_PicBackground.LoadFromRawFile("/gfx/background_hires.raw",
                                     640, 256, 3) == FALSE)
  {
    return;
  }

  struct RastPort* pRastPort = m_pGameView->RastPort();

  BltBitMapRastPort(m_PicBackground.GetBitMap(), 0, 0, pRastPort,
                    0, 0, 640, 256, 0xC0);

  //
  // Initialize postions of the Bobs and add them to the scene
  //
  m_pBobDuck->BobVSprite->X = 200;
  m_pBobDuck->BobVSprite->Y = 40;

  m_pBobHunter->BobVSprite->X = 20;
  m_pBobHunter->BobVSprite->Y = 222;

  AddBob(m_pBobDuck, pRastPort);
  AddBob(m_pBobHunter, pRastPort);

  //
  // Setting up some variables and the drawing rect for FPS display
  //                                   // TODO Fix design to avoid this *
  m_pPointsDisplay = new PointsDisplay(*m_pGameView, 1, 5, 5);

  m_pGameView->Render();

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

  short animFrameCnt = 1;
  bool bContinue = true;
  do
  {
    //
    // Move the hunter Bob depending on game pad interaction
    //
    ULONG portState = ReadJoyPort(1);
    if((portState & JP_TYPE_MASK) == JP_TYPE_JOYSTK)
    {
      if((portState & JPF_JOY_RIGHT) != 0)
      {
        m_pBobHunter->BobVSprite->X += 8;
        if(m_pBobHunter->BobVSprite->X > 640)
        {
          m_pBobHunter->BobVSprite->X = -16;
        }
      }
      else if((portState & JPF_JOY_LEFT) != 0)
      {
        m_pBobHunter->BobVSprite->X -= 8;
        if(m_pBobHunter->BobVSprite->X < 0)
        {
          m_pBobHunter->BobVSprite->X = 656;
        }
      }
    }

    //
    // Move the duck on an easy, linear right-to-left route
    //
    m_pBobDuck->BobVSprite->X -= 4;
    if(m_pBobDuck->BobVSprite->X < -40)
    {
      m_pBobDuck->BobVSprite->X = 650;
    }

    // Change the duck image every 2 frames
    if(animFrameCnt % 4 == 0)
    {
      m_BobDuck.NextImage();
      InitMasks(m_pBobDuck->BobVSprite);

      animFrameCnt = 0;
    }

    animFrameCnt++;

    // Render the changed scenery
    m_pGameView->Render();

    //
    // Calculate and update the FPS value
    //
    double dblElapsed = stopWatch.Pick();
    if(dblElapsed >= 0)
    {
      short fps = 1000 / dblElapsed;
      m_pPointsDisplay->UpdateFps(fps);
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
}
