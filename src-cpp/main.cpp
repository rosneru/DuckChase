#include <devices/timer.h>
#include <libraries/lowlevel.h>
#include <utility/tagitem.h>

#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/lowlevel_protos.h>

#include "animtools.h"
#include "animtools_proto.h"

#include "stdiostring.h"

#include "GameViewAdvanced.h"
#include "GelsBob.h"
#include "Picture.h"
#include "StopWatch.h"

void theGame(GameViewAdvanced& gameView);

void drawGels(GameViewAdvanced& gameView);
int cleanup(int);
int fail(STRPTR);

// The background picture and the bobs
Picture picBackgr;
GelsBob bobDuck(3, 59, 21, 3);
GelsBob bobHunter(3, 16, 22, 3);

int main(void)
{
  SetJoyPortAttrs(1,
                  SJA_Type, SJA_TYPE_AUTOSENSE,
                  TAG_END);

  SystemControl(SCON_TakeOverSys, TRUE,
                TAG_END);

  GameViewAdvanced gameView(640, 256, 3);
  if(gameView.Init() == false)
  {
    return fail((STRPTR)gameView.LastError());
  }

  // Initialize the GELs system
  struct GelsInfo* pGelsInfo = setupGelSys(gameView.GetRastPort(),
                                           0x03);

  if(pGelsInfo == NULL)
  {
    return fail("Could not initialize the Gels system\n");
  }

  //
  // Initialization is done, game can begin
  //
  theGame(gameView);

  // Free the resources allocated by the Gels system
  cleanupGelSys(pGelsInfo, gameView.GetRastPort());


  WaitTOF();
  WaitTOF();
  gameView.FreeAll();

  return cleanup(RETURN_OK);
}


void theGame(GameViewAdvanced& gameView)
{
  //
  // Setting the used color table (extracted from pic wit BtoC32)
  //
  USHORT colortable[8] =
  {
    0xAAA, 0x0, 0xFFF, 0x68B, 0x5A3, 0xEB0, 0xB52, 0xF80
  };

  // Change colors to those in colortable
  LoadRGB4(gameView.GetViewPort(), colortable, 8);


  //
  // Loading all the Bobs images
  //
  if(bobDuck.LoadImgFromRawFile("/gfx/ente1_hires.raw") == false)
  {
    return;
  }

  if(bobDuck.LoadImgFromRawFile("/gfx/ente2_hires.raw") == false)
  {
    return;
  }

  if(bobHunter.LoadImgFromRawFile("/gfx/jaeger1_hires.raw") == false)
  {
    return;
  }

  if(bobHunter.LoadImgFromRawFile("/gfx/jaeger2_hires.raw") == false)
  {
    return;
  }

  struct Bob* pBobDuck = bobDuck.Get();
  struct Bob* pBobHunter = bobHunter.Get();

  if((pBobDuck == NULL) || (pBobHunter == NULL))
  {
    return;
  }

  //
  // Load and display the background image
  //
  if(picBackgr.LoadFromRawFile("/gfx/background_hires.raw",
                               640, 256, 3) == FALSE)
  {
    return;
  }

  struct RastPort* pRastPort = gameView.GetRastPort();

  BltBitMapRastPort(picBackgr.GetBitMap(), 0, 0, pRastPort,
                    0, 0, 640, 256, 0xC0);

  //
  // Initialize postions of the Bobs and add them to the scene
  //
  pBobDuck->BobVSprite->X = 200;
  pBobDuck->BobVSprite->Y = 40;

  pBobHunter->BobVSprite->X = 20;
  pBobHunter->BobVSprite->Y = 222;

  AddBob(pBobDuck, pRastPort);
  AddBob(pBobHunter, pRastPort);

  //
  // Setting up some variables and the drawing rect for FPS display
  //
  char pFpsBuf[] = {"FPS: __________"};
  char* pFpsNumberStart = pFpsBuf + 5;

  SetBPen(pRastPort, 1);
  SetAPen(pRastPort, 1);
  RectFill(pRastPort, 0, 246, 639, 255);
  SetAPen(pRastPort, 5);

  drawGels(gameView);


  //
  // The main game loop
  //

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
        pBobHunter->BobVSprite->X += 8;
        if(pBobHunter->BobVSprite->X > 640)
        {
          pBobHunter->BobVSprite->X = -16;
        }
      }
      else if((portState & JPF_JOY_LEFT) != 0)
      {
        pBobHunter->BobVSprite->X -= 8;
        if(pBobHunter->BobVSprite->X < 0)
        {
          pBobHunter->BobVSprite->X = 656;
        }
      }
    }

    //
    // Move the duck on an easy, linear right-to-left route
    //
    pBobDuck->BobVSprite->X -= 4;
    if(pBobDuck->BobVSprite->X < -40)
    {
      pBobDuck->BobVSprite->X = 650;
    }

    // Change the duck image every 2 frames
    if(animFrameCnt % 4 == 0)
    {
      bobDuck.NextImage();
      InitMasks(pBobDuck->BobVSprite);

      animFrameCnt = 0;
    }

    animFrameCnt++;

    // draw the objects on thir new positions
    drawGels(gameView);

    //
    // Display the FPS value
    //
    double dblElapsed = stopWatch.Pick();

    if(dblElapsed >= 0)
    {
      // Calculatin fps and writing it to the string buf
      short fps = 1000 / dblElapsed;
      itoa(fps, pFpsNumberStart, 10);

      SetAPen(pRastPort, 1);
      RectFill(pRastPort, 550, 246, 639, 255);

      SetAPen(pRastPort, 5);
      Move(pRastPort, 550, 254);
      Text(pRastPort, pFpsBuf, strlength(pFpsBuf));
    }

    // Check if exit key ESC have been pressed
    ULONG key = GetKey();
    if((key & 0x00ff) == 0x45) // RAW code ESC key
    {
      bContinue = false;
    }
  }
  while (bContinue);

  RemBob(pBobHunter);
  RemBob(pBobDuck);
}

void drawGels(GameViewAdvanced& gameView)
{
  struct RastPort* pRastPort = gameView.GetRastPort();
  struct View* pView = gameView.GetView();

  SortGList(pRastPort);
  DrawGList(pRastPort, gameView.GetViewPort());
  WaitTOF();

  MrgCop(pView);
  LoadView(pView);

  // Switch the buffers
  gameView.SwitchBuffers();
}


/**
 * Print the error string and call cleanup() to exit
 */
int fail(STRPTR errorstring)
{
  VPrintf(errorstring, NULL);
  return cleanup(RETURN_FAIL);
}

/**
 * Fee everything that was allocated
 */
int cleanup(int returncode)
{
  SystemControl(SCON_TakeOverSys, FALSE,
                TAG_END);


  return returncode;
}
