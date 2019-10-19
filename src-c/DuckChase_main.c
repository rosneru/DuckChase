/**
 * Examples with good old GELs bobs.
 *
 * After half a day coding it looks as if it did become an ANSI-C99
 * variant of the DuckChase C++ game. I also think this fits better
 * to explore the possibilities of the Amiga Chipset when using
 * C and only the operating system.
 *
 * History
 *   17.10.2019 - Project start
 *
 * (c) 2019 Uwe Rosner
 */

#include <dos/dos.h>
#include <exec/types.h>
#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <libraries/lowlevel.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/lowlevel.h>

#include <stdio.h>
#include <string.h>

#include "animtools_proto.h"
#include "BitMapFunctions.h"
#include "LowlevelView.h"
#include "TimerFunctions.h"

/// User variables and settings

struct BitMap *m_pBackgrBM = NULL;

WORD *m_pDuck1ImageData = NULL;
WORD *m_pDuck2ImageData = NULL;
struct Bob *m_pDuckBob = NULL;

WORD *m_pHunter1ImageData = NULL;
struct Bob *m_pHunterBob = NULL;

USHORT m_PaletteBackgroundImg[8] =
{
  0x0, 0xAAA, 0xFFF, 0x68B, 0x5A3, 0xEB0, 0xB52, 0xF80
};

#define VP_WIDTH 640
#define VP_HEIGHT 256
#define VP_DEPTH 5

#define DUCK_WIDTH 59
#define DUCK_HEIGTH 21
#define DUCK_DEPTH 3
#define DUCK_WORDWIDTH 4

#define HUNTER_WIDTH 16
#define HUNTER_HEIGTH 22
#define HUNTER_DEPTH 3
#define HUNTER_WORDWIDTH 1

///

/// Privats and globals

extern struct GfxBase *GfxBase;

APTR m_pMemoryPoolChip = NULL;

struct View *m_pOldView;
struct View *m_pView;
struct ViewPort *m_pViewPort;

struct RastPort m_RastPort = {0};

struct GelsInfo *m_pGelsInfo;

/**
 * Initializes screen, window, duck bitmap & mask, backgr bitmap
 *
 * Returns NULL on success. When it fails it returns a pointer to an
 * error msg.
 */
char *initAll();

/**
 * Closes and un-initializes all in reverse order. Prints the given
 * error msg if non-zero.
 */
int cleanExit(char *pErrorMsg);

/**
 * Draws the bobs into the RastPort
 */
void drawBobGelsList(struct RastPort *pRPort, struct ViewPort *pVPort);

/**
 * Draws a small points at the bottom for points and fps
 */
void updatePointsDisplay(short fps, short strikes);

///

/// Main

int main(void)
{
  char *pErrorMsg = initAll();
  if (pErrorMsg != NULL)
  {
    return cleanExit(pErrorMsg);
  }

  // Load the palette of the background image
  LoadRGB4(m_pViewPort, m_PaletteBackgroundImg,
           sizeof(m_PaletteBackgroundImg) / sizeof(USHORT));

  // Blit the background image
  BltBitMapRastPort(m_pBackgrBM, 0, 0, &m_RastPort,
                    0, 0, 320, 256, 0xC0);

  // Initialize the points display
  updatePointsDisplay(0, 0);

  // Add the bobs and initially draw them
  AddBob(m_pDuckBob, &m_RastPort);
  AddBob(m_pHunterBob, &m_RastPort);
  drawBobGelsList(&m_RastPort, m_pViewPort);

  // Init LovLevel stuff
  SetJoyPortAttrs(1,
                  SJA_Type, SJA_TYPE_AUTOSENSE,
                  TAG_END);

  SystemControl(SCON_TakeOverSys, TRUE,
                TAG_END);

  BOOL bRun = TRUE;
  short counter = 0;

  //
  // Main loop. It runs until an intui message comes in.
  //
  StartTimer();
  do
  {
    // Every 5 frames change the duck image
    counter++;
    if (counter % 5 == 0)
    {
      if (m_pDuckBob->BobVSprite->ImageData == m_pDuck1ImageData)
      {
        m_pDuckBob->BobVSprite->ImageData = m_pDuck2ImageData;
      }
      else
      {
        m_pDuckBob->BobVSprite->ImageData = m_pDuck1ImageData;
      }
    }

    if (counter > 50)
    {
      counter = 0;
      double dblElapsed = PickTime(FALSE);

      if (dblElapsed >= 0)
      {
        // Sum the fps of each frame
        double fps = 50000 / dblElapsed;
        updatePointsDisplay((short)fps, 0);
      }
    }

    // The Duck is flying leftwards. When it leaves the screen it comes
    // in from the right again.
    m_pDuckBob->BobVSprite->X -= 5;
    if (m_pDuckBob->BobVSprite->X < -DUCK_WIDTH)
    {
      m_pDuckBob->BobVSprite->X = 656;
    }

    // The hunter is moved left or right with the joystick
    ULONG portState = ReadJoyPort(1);
    if ((portState & JP_TYPE_MASK) == JP_TYPE_JOYSTK)
    {
      if ((portState & JPF_JOY_RIGHT) != 0)
      {
        if (m_pHunterBob->BobVSprite->X + 10 > 640 + HUNTER_WIDTH)
        {
          m_pHunterBob->BobVSprite->X = HUNTER_WIDTH;
        }
        else
        {
          m_pHunterBob->BobVSprite->X += 10;
        }
      }
      else if ((portState & JPF_JOY_LEFT) != 0)
      {
        if (m_pHunterBob->BobVSprite->X - 10 < -HUNTER_WIDTH)
        {
          m_pHunterBob->BobVSprite->X = 640 + HUNTER_WIDTH;
        }
        else
        {
          m_pHunterBob->BobVSprite->X -= 10;
        }
      }
    }

    InitMasks(m_pDuckBob->BobVSprite);
    drawBobGelsList(&m_RastPort, m_pViewPort);

    ULONG key = GetKey();
    if ((key & 0x00ff) == 0x45) // RAW code ESC key
    {
      bRun = FALSE;
    }
  } while (bRun == TRUE);

  SystemControl(SCON_TakeOverSys, FALSE,
                TAG_END);

  return cleanExit(NULL);
}

///

/// Game

void updatePointsDisplay(short fps, short strikes)
{
  short backPen = 0;
  short frontPen = 5;

  if (fps == 0 && strikes == 0)
  {
    // fps and strikes = 0 -> init display

    // Drawing a filled black rect at the bottom of the view
    SetAPen(&m_RastPort, backPen);
    SetBPen(&m_RastPort, backPen);
    RectFill(&m_RastPort,
             0, VP_HEIGHT - 12,
             VP_WIDTH - 1, VP_HEIGHT - 1);
  }

  if (fps > 0)
  {
    char fpsBuf[16];
    sprintf(fpsBuf, "FPS: %d", fps);

    // Drawing a filled black rect at the bottom of the view
    SetAPen(&m_RastPort, backPen);
    SetBPen(&m_RastPort, backPen);
    RectFill(&m_RastPort,
             VP_WIDTH - 90, VP_HEIGHT - 12,
             VP_WIDTH - 4, VP_HEIGHT - 2);

    Move(&m_RastPort, VP_WIDTH - 90, VP_HEIGHT - 2 - 1);
    SetAPen(&m_RastPort, frontPen);
    Text(&m_RastPort, fpsBuf, strlen(fpsBuf));
  }

  if (strikes > 0)
  {
    // TODO
  }
}

void drawBobGelsList(struct RastPort *pRPort, struct ViewPort *pVPort)
{
  SortGList(pRPort);
  DrawGList(pRPort, pVPort);

  // If the GelsList includes true VSprites, MrgCop() and LoadView() here

  WaitTOF();
}

///

/// Init and cleanup

char *initAll()
{
  m_pMemoryPoolChip = CreatePool(MEMF_CHIP | MEMF_CLEAR, 200, 100);
  if (m_pMemoryPoolChip == NULL)
  {
    return ("Faild to create the memory pool!\n");
  }

  if (InitializeTimer() == FALSE)
  {
    return ("Faild to initialize the timer!\n");
  }

  // Load the background image
  m_pBackgrBM = LoadRawBitMap("background_lores.raw", 320, 256, 3);
  if (m_pBackgrBM == NULL)
  {
    return ("Failed to load background_lores.raw.\n");
  }

  // Load the duck
  m_pDuck1ImageData = LoadRawImageData(m_pMemoryPoolChip,
                                       "duck1_hires.raw",
                                       DUCK_WIDTH,
                                       DUCK_HEIGTH,
                                       DUCK_DEPTH);
  if (m_pDuck1ImageData == NULL)
  {
    return ("Failed to load duck1_hires.raw.\n");
  }

  m_pDuck2ImageData = LoadRawImageData(m_pMemoryPoolChip,
                                       "duck2_hires.raw",
                                       DUCK_WIDTH,
                                       DUCK_HEIGTH,
                                       DUCK_DEPTH);
  if (m_pDuck2ImageData == NULL)
  {
    return ("Failed to load duck2_hires.raw.\n");
  }

  // Load the hunter
  m_pHunter1ImageData = LoadRawImageData(m_pMemoryPoolChip,
                                         "hunter1_hires.raw",
                                         HUNTER_WIDTH,
                                         HUNTER_HEIGTH,
                                         HUNTER_DEPTH);

  if (m_pHunter1ImageData == NULL)
  {
    return ("Failed to load hunter1_hires.raw.\n");
  }

  // Init viewm, viewport and rasinfo
  m_pView = CreateAView(m_pMemoryPoolChip, PAL_MONITOR_ID | LORES_KEY);
  if (m_pView == NULL)
  {
    return ("Failed to create the view.\n");
  }

  m_pViewPort = CreateAViewPort(m_pMemoryPoolChip, VP_WIDTH,
                                VP_HEIGHT, VP_DEPTH,
                                PAL_MONITOR_ID | LORES_KEY);

  if (m_pViewPort == NULL)
  {
    return ("Faild to create the viewport.\n");
  }

  // Init rastport
  InitRastPort(&m_RastPort);

  // Link RastPort ands ViewPort
  m_RastPort.BitMap = m_pViewPort->RasInfo->BitMap;

  // Link View and ViewPort
  m_pView->ViewPort = m_pViewPort;

  // Construct preliminary Copper instruction list
  MakeVPort(m_pView, m_pViewPort);

  // Merge preliminary lists into a real Copper list in the View
  // structure
  MrgCop(m_pView);

  // Save current View to restore later
  m_pOldView = GfxBase->ActiView;

  LoadView(m_pView);
  WaitTOF();
  WaitTOF();


  // Init the gels system
  m_pGelsInfo = setupGelSys(&m_RastPort, 0x03);
  if (m_pGelsInfo == NULL)
  {
    return ("Failed to initialize the GELs system.\n");
  }

  // Create the bob objects for duck and hunter
  NEWBOB newBob =
  {
    m_pDuck1ImageData, DUCK_WORDWIDTH, DUCK_HEIGTH, DUCK_DEPTH,
    7,                  // Plane pick, 00111 enables all 3 low planes
    0,                  // Plane on off (unused planes to turn on)
    SAVEBACK | OVERLAY, // VSprite flags
    0,                  // Double buffering
    VP_DEPTH,           // Raster depth
    200, 40,            // initial x- and y-position
    0,                  // Hit mask
    0                   // Me mask
  };

  m_pDuckBob = makeBob(&newBob);
  if (m_pDuckBob == NULL)
  {
    return ("Failed to create GELs bob for duck.\n");
  }

  newBob.nb_Image = m_pHunter1ImageData;
  newBob.nb_WordWidth = HUNTER_WORDWIDTH;
  newBob.nb_LineHeight = HUNTER_HEIGTH;
  newBob.nb_ImageDepth = HUNTER_DEPTH;
  newBob.nb_X = 20;
  newBob.nb_Y = 222;

  m_pHunterBob = makeBob(&newBob);
  if (m_pHunterBob == NULL)
  {
    return ("Failed to create GELs bob for hunter.\n");
  }

  return NULL;
}

int cleanExit(char *pErrorMsg)
{
  if (m_pHunterBob != NULL)
  {
    RemBob(m_pHunterBob);
    drawBobGelsList(&m_RastPort, m_pViewPort);
    freeBob(m_pHunterBob, HUNTER_DEPTH);
    m_pHunterBob = NULL;
  }

  if (m_pDuckBob != NULL)
  {
    RemBob(m_pDuckBob);
    drawBobGelsList(&m_RastPort, m_pViewPort);
    freeBob(m_pDuckBob, DUCK_DEPTH);
    m_pDuckBob = NULL;
  }

  if (m_pGelsInfo != NULL)
  {
    cleanupGelSys(m_pGelsInfo, &m_RastPort);
    m_pGelsInfo = NULL;
  }

  // Restore the old view
  if (GfxBase->ActiView == m_pView)
  {
    // Put back the old view
    LoadView(m_pOldView);

    // Before freeing memory wait until the old view is being rendered
    WaitTOF();
    WaitTOF();
  }

  // Delete the custom view and viewport
  DeleteAViewPort(m_pViewPort);
  DeleteAView(m_pView);

  if (m_pBackgrBM != NULL)
  {
    FreeRawBitMap(m_pBackgrBM);
    m_pBackgrBM = NULL;
  }

  FreeTimer();

  if (m_pMemoryPoolChip != NULL)
  {
    DeletePool(m_pMemoryPoolChip);
    m_pMemoryPoolChip = NULL;
  }

  if (pErrorMsg == NULL)
  {
    return RETURN_OK;
  }
  else
  {
    printf(pErrorMsg);
    return RETURN_ERROR;
  }
}

///
