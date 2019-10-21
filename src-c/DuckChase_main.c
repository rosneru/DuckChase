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

/// Includes

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

///

/// User settings

/**
 * Holds the game speed according the performance of the current 
 * Amiga system. Is re-calculated every 50 frames.
 * 
 * Values:
 * 0 when current fps is in range [0, 20]
 * 1 when current fps is in range [21, 40]
 * 2 when current fps is in range [40, upwards]
 */
short gameSpeed = 1;

// For each gameSpeed the different entities move with a different
// speed (pixel-per-frame)
short hunterDX[] = {19, 15, 10};
short hunterDY[] = {0, 0, 0}; // Not used yet

short duckDX[] = {10, 8, 5};
short duckDY[] = {0, 0, 0}; // Not used yet

short arrowDX[] = {15, 12, 8};
short arrowDY[] = {8, 6, 4};

// For each gameSpeed the switching of the images of some entities can
// be adjusted here. The value means after how many frames the image
// is switched
short hunterImgSwitch[] = {2, 3, 5};
short duckImgSwitch[] = {2, 3, 5};

#define VP_PALETTE_SIZE 32

ULONG m_PaletteBackgroundImg[] =
    {
        0x00100000,
        0x00000000, 0x00000000, 0x00000000, 0x28282828, 0x28282828, 0x28282828,
        0x50505050, 0x49494949, 0x45454545, 0x7C7C7C7C, 0x6F6F6F6F, 0x66666666,
        0xFBFBFBFB, 0xF1F1F1F1, 0xC7C7C7C7, 0x46464646, 0x85858585, 0x88888888,
        0x83838383, 0xA5A5A5A5, 0x98989898, 0x68686868, 0x9D9D9D9D, 0x6A6A6A6A,
        0xAEAEAEAE, 0xC0C0C0C0, 0x7C7C7C7C, 0x98989898, 0x97979797, 0x1A1A1A1A,
        0xB8B8B8B8, 0xBBBBBBBB, 0x24242424, 0xD7D7D7D7, 0x99999999, 0x21212121,
        0xFAFAFAFA, 0xBDBDBDBD, 0x2F2F2F2F, 0xD6D6D6D6, 0x5D5D5D5D, 0xE0E0E0E,
        0xCCCCCCCC, 0x24242424, 0x1D1D1D1D, 0xFBFBFBFB, 0x49494949, 0x34343434,
        0x00000000};

// Settings for the display
#define VP_WIDTH 640
#define VP_HEIGHT 256
#define VP_DEPTH 4
#define VP_MODE (PAL_MONITOR_ID | HIRES_KEY)

// Settings for the bobs and sprites
#define DUCK_WIDTH 59
#define DUCK_HEIGTH 21
#define DUCK_DEPTH 4
#define DUCK_WORDWIDTH 4

#define HUNTER_WIDTH 25
#define HUNTER_HEIGTH 27
#define HUNTER_DEPTH 4
#define HUNTER_WORDWIDTH 2

///

/// Private globals variables

short hunterFrameCnt = 0;
short duckFrameCnt = 0;

WORD *m_pDuck1ImageData = NULL;
WORD *m_pDuck2ImageData = NULL;
struct Bob *m_pDuckBob = NULL;

WORD *m_pHunter1ImageData = NULL;
WORD *m_pHunter2ImageData = NULL;
struct Bob *m_pHunterBob = NULL;

struct BitMap *m_pBackgrBM = NULL;

struct Screen *m_pDummyScreen;
struct View *m_pOldView;
struct View *m_pView;
struct ViewPort *m_pViewPort;
struct RastPort m_RastPort = {0};
struct GelsInfo *m_pGelsInfo;

APTR m_pMemoryPoolChip = NULL;
extern struct GfxBase *GfxBase;

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

/**
 * Update function for the hunter.
 * 
 * It is called everey frame. 
 */
void updateHunter(ULONG portState);

/**
 * Update function for the duck.
 * 
 * It is called everey frame.
 */
void updateDuck();

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
  LoadRGB32(m_pViewPort, m_PaletteBackgroundImg);

  // Blit the background image
  BltBitMapRastPort(m_pBackgrBM, 0, 0, &m_RastPort,
                    0, 0, VP_WIDTH, 256, 0xC0);

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
  // Main loop.
  //
  StartTimer();
  do
  {
    // Every 50 frames update the fps dsiplay and re-calculate the game
    // speed
    if (counter > 50)
    {
      counter = 0;
      double dblElapsed = PickTime(FALSE);
      if (dblElapsed >= 0)
      {
        // Sum the fps of each frame
        short fps = (short)(50000 / dblElapsed);
        updatePointsDisplay(fps, 0);

        // Update game speed according current fps
        if (fps > 40)
        {
          gameSpeed = 2;
        }
        else if (fps > 20)
        {
          gameSpeed = 1;
        }
        else
        {
          gameSpeed = 0;
        }
      }
    }

    ULONG portState = ReadJoyPort(1);
    updateDuck();
    updateHunter(portState);

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

  RemBob(m_pHunterBob);
  RemBob(m_pDuckBob);
  drawBobGelsList(&m_RastPort, m_pViewPort);

  return cleanExit(NULL);
}

void drawBobGelsList(struct RastPort *pRPort, struct ViewPort *pVPort)
{
  SortGList(pRPort);
  DrawGList(pRPort, pVPort);

  // If the GelsList includes true VSprites, MrgCop() and LoadView() here

  WaitTOF();
}

///

/// Game

void updateHunter(ULONG portState)
{
  // Get the VSprite for better readabilty of the following code
  struct VSprite *vSprite = m_pHunterBob->BobVSprite;

  // The hunter is moved left or right with the joystick
  if ((portState & JP_TYPE_MASK) == JP_TYPE_JOYSTK)
  {
    if ((portState & JPF_JOY_RIGHT) != 0)
    {
      if (vSprite->X + hunterDX[gameSpeed] > VP_WIDTH + HUNTER_WIDTH)
      {
        vSprite->X = -HUNTER_WIDTH;
      }
      else
      {
        vSprite->X += 10;
      }
    }
    else if ((portState & JPF_JOY_LEFT) != 0)
    {
      if (vSprite->X - 10 < -HUNTER_WIDTH)
      {
        vSprite->X = VP_WIDTH + HUNTER_WIDTH;
      }
      else
      {
        vSprite->X -= 10;
      }
    }
  }
}

void updateDuck()
{
  // Get the VSprite for better readabilty of the following code
  struct VSprite *vSprite = m_pDuckBob->BobVSprite;

  // Every some frames change the duck image
  duckFrameCnt++;
  if (duckFrameCnt % duckImgSwitch[gameSpeed] == 0)
  {
    duckFrameCnt = 0;
    if (vSprite->ImageData == m_pDuck1ImageData)
    {
      vSprite->ImageData = m_pDuck2ImageData;
    }
    else
    {
      vSprite->ImageData = m_pDuck1ImageData;
    }
  }

  // The Duck is flying leftwards. When it leaves the screen it comes
  // in from the right again.
  vSprite->X -= duckDX[gameSpeed];
  if (vSprite->X < -DUCK_WIDTH)
  {
    vSprite->X = VP_WIDTH + 16;
  }
}

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
  m_pBackgrBM = LoadRawBitMap("raw/background.raw",
                              VP_WIDTH, VP_HEIGHT, VP_DEPTH);
  if (m_pBackgrBM == NULL)
  {
    return ("Failed to load background.raw.\n");
  }

  // Load the duck
  m_pDuck1ImageData = LoadRawImageData(m_pMemoryPoolChip,
                                       "raw/duck1.raw",
                                       DUCK_WIDTH,
                                       DUCK_HEIGTH,
                                       DUCK_DEPTH);
  if (m_pDuck1ImageData == NULL)
  {
    return ("Failed to load duck1.raw.\n");
  }

  m_pDuck2ImageData = LoadRawImageData(m_pMemoryPoolChip,
                                       "raw/duck2.raw",
                                       DUCK_WIDTH,
                                       DUCK_HEIGTH,
                                       DUCK_DEPTH);
  if (m_pDuck2ImageData == NULL)
  {
    return ("Failed to load duck2.raw.\n");
  }

  // Load the hunter img data #1
  m_pHunter1ImageData = LoadRawImageData(m_pMemoryPoolChip,
                                         "raw/hunter_right1.raw",
                                         HUNTER_WIDTH,
                                         HUNTER_HEIGTH,
                                         HUNTER_DEPTH);

  if (m_pHunter1ImageData == NULL)
  {
    return ("Failed to load hunter_right1.RawDoFmt(.\n");
  }

  // Load the hunter img data #2
  m_pHunter2ImageData = LoadRawImageData(m_pMemoryPoolChip,
                                         "raw/hunter_right2.raw",
                                         HUNTER_WIDTH,
                                         HUNTER_HEIGTH,
                                         HUNTER_DEPTH);

  if (m_pHunter2ImageData == NULL)
  {
    return ("Failed to load hunter_right2.raw.\n");
  }

  // This screen is only a trick: It just exists to ensure that after
  // closing the main view *Intuition* handles important things like
  // giving back the mouse pointer etc. which were lost by creating
  // our own view. So it is only opened with a depth of 1 losing about
  // 20k in HiRes or 10 in LoRes.
  m_pDummyScreen = OpenScreenTags(NULL,
                                  SA_DisplayID, VP_MODE,
                                  SA_Depth, 1,
                                  SA_Width, VP_WIDTH,
                                  SA_Height, VP_HEIGHT,
                                  SA_ShowTitle, FALSE,
                                  TAG_DONE);

  if (m_pDummyScreen == NULL)
  {
    return ("Faild to open the dummy screen!\n");
  }

  // Init view, viewport and rasinfo
  m_pView = CreateAView(m_pMemoryPoolChip, VP_MODE);
  if (m_pView == NULL)
  {
    return ("Failed to create the view.\n");
  }

  m_pViewPort = CreateAViewPort(m_pMemoryPoolChip, VP_WIDTH,
                                VP_HEIGHT, VP_DEPTH,
                                VP_MODE, VP_PALETTE_SIZE);

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

  // Create the bob ObtainBattSemaphore(jects for duck and hunter
  NEWBOB newBob =
      {
          m_pDuck1ImageData, DUCK_WORDWIDTH, DUCK_HEIGTH, DUCK_DEPTH,
          15,                 // Plane pick, 01111 enables all 4 low planes
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
    freeBob(m_pHunterBob, HUNTER_DEPTH);
    m_pHunterBob = NULL;
  }

  if (m_pDuckBob != NULL)
  {
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
  if (m_pViewPort != NULL)
  {
    DeleteAViewPort(m_pViewPort);
  }

  if (m_pView != NULL)
  {
    DeleteAView(m_pView);
  }

  if (m_pDummyScreen != NULL)
  {
    CloseScreen(m_pDummyScreen);
    m_pDummyScreen = NULL;
  }

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
