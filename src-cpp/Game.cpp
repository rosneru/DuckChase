#include <clib/graphics_protos.h>
#include <clib/lowlevel_protos.h>
#include <libraries/lowlevel.h>

#include "animtools_proto.h"
#include "StopWatch.h"

#include "Game.h"


Game::Game(IGameView& gameView)
  : m_GameView(gameView),
    m_PointsDisplay(m_GameView, 1, 5, 5, 3),
    m_pGelsInfo(NULL),
    m_pLastError(NULL),
    m_BobDuck(m_GameView.Depth(), 59, 21, 3),
    m_BobHunter(m_GameView.Depth(), 16, 22, 3),
    m_SpriteBullet(16, 13),
    m_pBobDuck(NULL),
    m_pBobHunter(NULL),
    m_pSpriteBullet(NULL)
{

}


Game::~Game()
{
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

  if(m_SpriteBullet.LoadImgFromRawFile("/gfx/bullet1_hires.raw") == false)
  {
    m_pLastError = "Couldn't load sprite bullet image #1 /gfx/bullet1_hires.raw";
    return false;
  }

  // Load more pictures for bullet sprite to allow animating them
  m_SpriteBullet.LoadImgFromRawFile("/gfx/bullet2_hires.raw");
  m_SpriteBullet.LoadImgFromRawFile("/gfx/bullet3_hires.raw");
  m_SpriteBullet.LoadImgFromRawFile("/gfx/bullet4_hires.raw");
  m_SpriteBullet.LoadImgFromRawFile("/gfx/bullet5_hires.raw");
  m_SpriteBullet.LoadImgFromRawFile("/gfx/bullet6_hires.raw");
  m_SpriteBullet.LoadImgFromRawFile("/gfx/bullet7_hires.raw");
  m_SpriteBullet.LoadImgFromRawFile("/gfx/bullet8_hires.raw");


  m_pBobDuck = m_BobDuck.Get();
  m_pBobHunter = m_BobHunter.Get();
  m_pSpriteBullet = m_SpriteBullet.Get();

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

  if(m_pSpriteBullet == NULL)
  {
    m_pLastError = "Couldn't acquire bullet sprite\n";
    return false;
  }

  // Set the colors for the gsprite
  ULONG colorsBulletSprite[4][3] =
  {
    {0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA},
    {0xE5E5E5E5, 0x14141414, 0x1D1D1D1D},
    {0xC2C2C2C2, 0x5A5A5A5A, 0x20202020},
    {0x2E2E2E2E, 0x14141414, 0x9090909},
  };

  int spriteNum = m_SpriteBullet.SpriteNumber();
  int spriteColRegStart = 16 + ((spriteNum & 0x06) << 1);
  for(int i = spriteColRegStart; i < (spriteColRegStart + 4); i++)
  {
    int r = colorsBulletSprite[i - spriteColRegStart][0];
    int g = colorsBulletSprite[i - spriteColRegStart][1];
    int b = colorsBulletSprite[i - spriteColRegStart][2];
    SetRGB32(m_GameView.ViewPort(), i, r, g, b);
  }


  //
  // Load and display the background image
  //
  if(m_PicBackground.LoadFromRawFile("/gfx/background_hires.raw",
                                     640, 256, 3) == FALSE)
  {
    m_pLastError = "Couldn't load background image\n";
    return false;
  }

  struct RastPort* pRastPort = m_GameView.RastPort();

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

  // TODO Do not cast, use field!!
  MoveSprite(m_GameView.ViewPort(),
             (struct SimpleSprite*)m_pSpriteBullet,
             300, 244);
  //
  // Setting up some variables and the drawing rect for FPS display
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
  short animFrameCnt = 1;
  short fpsCnt = 0;
  long fps = 0;

  StopWatch stopWatch;
  stopWatch.Start();

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

    if(animFrameCnt % 4 == 0)
    {
      // Change the bullet sprite image every frame
      struct ExtSprite* pCurrSpriteImg = m_pSpriteBullet;
      m_SpriteBullet.NextImage();
      m_pSpriteBullet = m_SpriteBullet.Get();
      ChangeExtSprite(m_GameView.ViewPort(),
                      pCurrSpriteImg,
                      m_pSpriteBullet,
                      TAG_END);
    }

    animFrameCnt++;

    // Render the changed scenery
    m_GameView.Render();

    //
    // Calculate and update the FPS value
    //
    double dblElapsed = stopWatch.Pick();
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
