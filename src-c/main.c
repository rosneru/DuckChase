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
#include <graphics/videocontrol.h>
#include <intuition/intuition.h>
#include <libraries/lowlevel.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/lowlevel.h>

#include <stdio.h>
#include <string.h>

#include "animtools_proto.h"
#include "imagetools.h"
#include "timertools.h"

///

/// User accessible settings

// Settings for the display
#define VP_WIDTH 640
#define VP_HEIGHT 256
#define VP_DEPTH 4
#define VP_MODE (PAL_MONITOR_ID | HIRES_KEY)


/**
 * A container for holding all needed data for ImageData based entities
 */
typedef struct ImageContainer
{
  const char* pImgPath;
  WORD width;
  WORD height;
  SHORT depth;
  WORD* pImageData;
} ImageContainer;

/**
 * A container for holding all needed data for BitMap based entities
 */
typedef struct BitMapContainer
{
  const char* pImgPath;
  WORD width;
  WORD height;
  SHORT depth;
  struct BitMap* pBitMap;
} BitMapContainer;

ImageContainer duckImages[] =
{
  // Dimensions only defined for the first item; all following share
  {"raw/duck1.raw", 59, 21, 4, NULL},
  {"raw/duck2.raw", 0, 0, 0, NULL}
};

ImageContainer hunterImages[] =
{
  // Dimensions only defined for the first item; all following share
  {"raw/hunter_right1.raw", 25, 27, 4, NULL},
  {"raw/hunter_right2.raw", 0, 0, 0, NULL},
  {"raw/hunter_right_shoot.raw", 0, 0, 0, NULL},
  {"raw/hunter_Left1.raw", 0, 0, 0, NULL},
  {"raw/hunter_Left2.raw", 0, 0, 0, NULL},
  {"raw/hunter_Left_shoot.raw", 0, 0, 0, NULL}
};

BitMapContainer arrowRImages[] =
{
  // Dimensions only defined for the first item; all following share
  {"raw/arrow_right1.raw", 16, 8, 2, NULL}
};

BitMapContainer arrowLImages[] =
{
  // Dimensions only defined for the first item; all following share
  {"raw/arrow_left1.raw", 16, 8, 2, NULL}
};


/**
 * Holds the game speed according the performance of the current
 * Amiga system. Is re-calculated every 50 frames.
 *
 * Values:
 * 0 when current fps is in range [0, 20]
 * 1 when current fps is in range [21, 40]
 * 2 when current fps is in range [40, upwards]
 */
SHORT gameSpeed = 1;

// Depending on gameSpeed the entities can move with a different speed
// (in pixel-per-frame)
SHORT hunterDX[] = {9, 6, 3};
SHORT hunterDY[] = {0, 0, 0}; // Not used yet

SHORT duckDX[] = {15, 10, 5};
SHORT duckDY[] = {0, 0, 0}; // Not used yet

SHORT arrowDX[] = {15, 10, 5};
SHORT arrowDY[] = {8, 6, 4};

// For each gameSpeed the switching rate of the images of some
// entities can be adjusted here. The value means after how many
// frames the image is switched.
SHORT hunterImgSwitch[] = {4, 6, 8};
SHORT duckImgSwitch[] = {4, 6, 8};


struct Bob* m_pDuckBob = NULL;
struct Bob* m_pHunterBob = NULL;
struct ExtSprite* m_pArrowSprite = NULL;
LONG m_SpriteNumber = -1;

#define VP_PALETTE_SIZE 32

ULONG m_PaletteBackgroundImg[] =
{
  0x00200000,
  0x00000000, 0x00000000, 0x00000000, 0x28282828, 0x28282828, 0x28282828,
  0x50505050, 0x49494949, 0x45454545, 0x7C7C7C7C, 0x6F6F6F6F, 0x66666666,
  0xFBFBFBFB, 0xF1F1F1F1, 0xC7C7C7C7, 0x46464646, 0x85858585, 0x88888888,
  0x83838383, 0xA5A5A5A5, 0x98989898, 0x68686868, 0x9D9D9D9D, 0x6A6A6A6A,
  0xAEAEAEAE, 0xC0C0C0C0, 0x7C7C7C7C, 0x98989898, 0x97979797, 0x1A1A1A1A,
  0xB8B8B8B8, 0xBBBBBBBB, 0x24242424, 0xD7D7D7D7, 0x99999999, 0x21212121,
  0xFAFAFAFA, 0xBDBDBDBD, 0x2F2F2F2F, 0xD6D6D6D6, 0x5D5D5D5D, 0xE0E0E0E,
  0xCCCCCCCC, 0x24242424, 0x1D1D1D1D, 0xFBFBFBFB, 0x49494949, 0x34343434,

  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,

  0x00000000
};

ULONG m_SpriteColors[] =
{
  0x28282828, 0x28282828, 0x28282828,
  0x1B1B1B1B, 0x38383838, 0x59595959,
  0xD6D6D6D6, 0x5D5D5D5D, 0x0E0E0E0E,
};

///

/// Private global variables

struct BitMap* m_pBackgrBM = NULL;

struct Screen* m_pScreen = NULL;
struct GelsInfo* m_pGelsInfo;

APTR m_pMemoryPoolChip = NULL;
extern struct GfxBase* GfxBase;

/**
 * Initializes screen, window, duck bitmap & mask, backgr bitmap
 *
 * Returns NULL on success. When it fails it returns a pointer to an
 * error msg.
 */
char* initAll();

/**
 * Loading numItems images in the given container.
 *
 * Returns TRUE on success.
 *
 * In error case its prints the file name of the unloadable file to the
 * console and returns FALSE.
 */
BOOL loadAllImages(struct ImageContainer* pContainer, int numItems);

/**
 * Loading numItems images in the given container.
 *
 * Returns TRUE on success.
 *
 * In error case its prints the file name of the unloadable file to the
 * console and returns FALSE.
 */
BOOL loadAllBitMaps(struct BitMapContainer* pContainer, int numItems);


/**
 * Closes and un-initializes all in reverse order. Prints the given
 * error msg if non-zero.
 */
int cleanExit(char* pErrorMsg);

/**
 * Draws the bobs into the RastPort
 */
void drawBobGelsList(struct RastPort* pRPort, struct ViewPort* pVPort);

/**
 * Calculates the number of 16-bit-words needed for a given amount of
 * bits.
 */
ULONG bitsToWords(ULONG bits);

/**
 * Draws a small points at the bottom for points and fps
 */
void updatePointsDisplay(SHORT fps, SHORT strikes);

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

  // Blit the background image
  BltBitMapRastPort(m_pBackgrBM, 0, 0, &m_pScreen->RastPort,
                    0, 0, VP_WIDTH, 256, 0xC0);

  // Initialize the points display
  updatePointsDisplay(0, 0);

  // Add the bobs and initially draw them
  AddBob(m_pDuckBob, &m_pScreen->RastPort);
  AddBob(m_pHunterBob, &m_pScreen->RastPort);
  drawBobGelsList(&m_pScreen->RastPort, &m_pScreen->ViewPort);

  // Init LovLevel stuff
  SetJoyPortAttrs(1,
                  SJA_Type, SJA_TYPE_AUTOSENSE,
                  TAG_END);

  SystemControl(SCON_TakeOverSys, TRUE,
                TAG_END);

  BOOL bRun = TRUE;
  SHORT counter = 0;

  //
  // Main loop.
  //
  StartTimer();
  do
  {
    // Every 50 frames update the fps dsiplay and re-calculate the game
    // speed
    counter++;
    if (counter > 50)
    {
      counter = 0;
      double dblElapsed = PickTime(FALSE);
      if (dblElapsed >= 0)
      {
        // Sum the fps of each frame
        SHORT fps = (SHORT)(50000 / dblElapsed);
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
    InitMasks(m_pHunterBob->BobVSprite);
    drawBobGelsList(&m_pScreen->RastPort, &m_pScreen->ViewPort);

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
  drawBobGelsList(&m_pScreen->RastPort, &m_pScreen->ViewPort);

  return cleanExit(NULL);
}

void drawBobGelsList(struct RastPort *pRPort, struct ViewPort *pVPort)
{
  SortGList(pRPort);
  WaitTOF();
  DrawGList(pRPort, pVPort);

//  MrgCop(m_pView);
//  LoadView(m_pView);
}

ULONG bitsToWords(ULONG bits)
{
  ULONG integers = bits / 16;
  if((bits % 16) == 0)
  {
    return integers;
  }
  else
  {
    return integers + 1;
  }
}

///

/// Game

SHORT hunterFrameCnt = 0;
ULONG m_HunterLastDirection = JPF_JOY_RIGHT;
BOOL m_bHunterLaunchesArrow = FALSE;
BOOL m_bHunterRunning = FALSE;

void updateHunter(ULONG portState)
{
  // Get the VSprite for better readabilty of the following code
  struct VSprite *vSprite = m_pHunterBob->BobVSprite;

  // The hunter is moved left or right with the joystick
  BOOL bDirectionChanged = FALSE;
  if ((portState & JPF_JOY_RIGHT) != 0)
  {
    m_bHunterLaunchesArrow = FALSE;
    m_bHunterRunning = TRUE;

    // Check if direction has changed
    if(m_HunterLastDirection != JPF_JOY_RIGHT)
    {
      bDirectionChanged = TRUE;
      hunterFrameCnt = 0;
      m_HunterLastDirection = JPF_JOY_RIGHT;
    }

    // Move the hunter to right
    if (vSprite->X + hunterDX[gameSpeed] > VP_WIDTH + hunterImages[0].width)
    {
      vSprite->X = -hunterImages[0].width;
    }
    else
    {
      vSprite->X += hunterDX[gameSpeed];
    }

    // Every some frames (or if the direction changed) switch the
    // hunter image
    hunterFrameCnt++;
    if ((bDirectionChanged == TRUE) ||
        (hunterFrameCnt % hunterImgSwitch[gameSpeed] == 0))
    {
      hunterFrameCnt = 0;
      if (vSprite->ImageData == hunterImages[0].pImageData) // TODO remove hard coded numbers
      {
        vSprite->ImageData = hunterImages[1].pImageData;
      }
      else
      {
        vSprite->ImageData = hunterImages[0].pImageData;
      }
    }
  }
  else if ((portState & JPF_JOY_LEFT) != 0)
  {
    m_bHunterLaunchesArrow = FALSE;
    m_bHunterRunning = TRUE;

    // Check if direction has changed
    if(m_HunterLastDirection != JPF_JOY_LEFT)
    {
      bDirectionChanged = TRUE;
      hunterFrameCnt = 0;
      m_HunterLastDirection = JPF_JOY_LEFT;
    }

    // Move the hunter to right
    if (vSprite->X - hunterDX[gameSpeed] < -hunterImages[0].width)
    {
      vSprite->X = VP_WIDTH + hunterImages[0].width;
    }
    else
    {
      vSprite->X -= hunterDX[gameSpeed];
    }

    // Every some frames (or if the direction changed) switch the
    // hunter image
    hunterFrameCnt++;
    if ((bDirectionChanged == TRUE) ||
        (hunterFrameCnt % hunterImgSwitch[gameSpeed] == 0))
    {
      hunterFrameCnt = 0;
      if (vSprite->ImageData == hunterImages[3].pImageData) // TODO remove hard coded numbers
      {
        vSprite->ImageData = hunterImages[4].pImageData;
      }
      else
      {
        vSprite->ImageData = hunterImages[3].pImageData;
      }
    }
  }
  else if((portState & JPF_BTN2) != 0)
  {
    m_bHunterLaunchesArrow = TRUE;
    m_bHunterRunning = FALSE;

    if(m_HunterLastDirection == JPF_RIGHT)
    {
      vSprite->ImageData = hunterImages[2].pImageData;      // TODO remove hard coded numbers
    }
    else
    {
      vSprite->ImageData = hunterImages[5].pImageData;
    }
  }
  else
  {
    if(m_bHunterLaunchesArrow == TRUE)
    {
      m_bHunterLaunchesArrow = FALSE;
      if(m_HunterLastDirection == JPF_RIGHT)
      {
        vSprite->ImageData = hunterImages[0].pImageData;
      }
      else
      {
        vSprite->ImageData = hunterImages[3].pImageData;
      }
    }
    else if(m_bHunterRunning == TRUE)
    {
      m_bHunterRunning = FALSE;

      if(m_HunterLastDirection == JPF_RIGHT)
      {
        vSprite->ImageData = hunterImages[0].pImageData;
      }
      else
      {
        vSprite->ImageData = hunterImages[3].pImageData;
      }
    }
  }
}


SHORT duckFrameCnt = 0;

void updateDuck()
{
  // Get the VSprite for better readabilty of the following code
  struct VSprite *vSprite = m_pDuckBob->BobVSprite;

  // Every some frames change the duck image
  duckFrameCnt++;
  if (duckFrameCnt % duckImgSwitch[gameSpeed] == 0)
  {
    duckFrameCnt = 0;
    if (vSprite->ImageData == duckImages[0].pImageData)
    {
      vSprite->ImageData = duckImages[1].pImageData;
      MoveSprite(&m_pScreen->ViewPort, (struct SimpleSprite*) m_pArrowSprite, 50, 100);

    }
    else
    {
      vSprite->ImageData = duckImages[0].pImageData;
      MoveSprite(&m_pScreen->ViewPort, (struct SimpleSprite*) m_pArrowSprite, 100, 100);

    }
  }

  // The Duck is flying leftwards. When it leaves the screen it comes
  // in from the right again.
  vSprite->X -= duckDX[gameSpeed];
  if (vSprite->X < -duckImages[0].width)
  {
    vSprite->X = VP_WIDTH + 16;
  }
}

void updatePointsDisplay(SHORT fps, SHORT strikes)
{
  SHORT backPen = 0;
  SHORT frontPen = 5;

  if (fps == 0 && strikes == 0)
  {
    // fps and strikes = 0 -> init display

    // Drawing a filled black rect at the bottom of the view
    SetAPen(&m_pScreen->RastPort, backPen);
    SetBPen(&m_pScreen->RastPort, backPen);
    RectFill(&m_pScreen->RastPort,
             0, VP_HEIGHT - 12,
             VP_WIDTH - 1, VP_HEIGHT - 1);
  }

  if (fps > 0)
  {
    char fpsBuf[16];
    sprintf(fpsBuf, "FPS: %d", fps);

    // Drawing a filled black rect at the bottom of the view
    SetAPen(&m_pScreen->RastPort, backPen);
    SetBPen(&m_pScreen->RastPort, backPen);
    RectFill(&m_pScreen->RastPort,
             VP_WIDTH - 90, VP_HEIGHT - 12,
             VP_WIDTH - 4, VP_HEIGHT - 2);

    Move(&m_pScreen->RastPort, VP_WIDTH - 90, VP_HEIGHT - 2 - 1);
    SetAPen(&m_pScreen->RastPort, frontPen);
    Text(&m_pScreen->RastPort, fpsBuf, strlen(fpsBuf));
  }

  if (strikes > 0)
  {
    // TODO
  }
}

///

/// Init and cleanup

BOOL loadAllImages(struct ImageContainer* pContainer, int numItems)
{
  for(int i = 0; i < numItems; i++)
  {
    pContainer[i].pImageData = LoadRawImageData(m_pMemoryPoolChip,
                                                pContainer[i].pImgPath,
                                                pContainer[0].width,
                                                pContainer[0].height,
                                                pContainer[0].depth);

    if(pContainer[i].pImageData == NULL)
    {
      printf("Failed to load '%s'. ", pContainer[i].pImgPath);
      return FALSE;
    }
  }

  return TRUE;
}

BOOL loadAllBitMaps(struct BitMapContainer* pContainer, int numItems)
{
  for(int i = 0; i < numItems; i++)
  {
    pContainer[i].pBitMap = LoadRawBitMap(pContainer[i].pImgPath,
                                          pContainer[0].width,
                                          pContainer[0].height,
                                          pContainer[0].depth);

    if(pContainer[i].pBitMap == NULL)
    {
      printf("Failed to load '%s'. ", pContainer[i].pImgPath);
      return FALSE;
    }
  }

  return TRUE;
}


char* initAll()
{
  //
  // Create memory pool
  //
  m_pMemoryPoolChip = CreatePool(MEMF_CHIP | MEMF_CLEAR, 1024, 512);
  if (m_pMemoryPoolChip == NULL)
  {
    return ("Faild to create the memory pool!\n");
  }

  if (InitializeTimer() == FALSE)
  {
    return ("Faild to initialize the timer!\n");
  }

  //
  // Load all images
  //

  // Load the background image
  m_pBackgrBM = LoadRawBitMap("raw/background.raw",
                              VP_WIDTH, VP_HEIGHT, 4);
  if (m_pBackgrBM == NULL)
  {
    return ("Failed to load background.raw.\n");
  }

  // Load the duck images
  int numDuckImages = sizeof duckImages / sizeof duckImages[0];
  if(loadAllImages(duckImages, numDuckImages) == FALSE)
  {
    return("Init error.\n");
  }

  // Load the hunter images
  int numHunterImages = sizeof hunterImages / sizeof hunterImages[0];
  if(loadAllImages(hunterImages, numHunterImages) == FALSE)
  {
    return("Init error.\n");
  }

  // Load the arrow right images
  int numRArrows = sizeof arrowRImages / sizeof arrowRImages[0];
  if(loadAllBitMaps(arrowRImages, numRArrows) == FALSE)
  {
    return("Init error.\n");
  }

  // Load the arrow left images
  int numLArrows = sizeof arrowLImages / sizeof arrowLImages[0];
  if(loadAllBitMaps(arrowLImages, numLArrows) == FALSE)
  {
    return("Init error.\n");
  }

  // Create the arrow sprite
  m_pArrowSprite = AllocSpriteData(arrowRImages[0].pBitMap,
                                   SPRITEA_Width, arrowRImages[0].width,
                                   TAG_END);

  if(m_pArrowSprite == NULL)
  {
    return ("Failed to allocate sprite data.\n");
  }

  m_SpriteNumber = GetExtSprite(m_pArrowSprite,
                                TAG_END);

  if(m_SpriteNumber < 0)
  {
    return("Failed to acquire a hardware sprite.\n");
  }

  //
  // Open the screen
  //

  // Give the screen an additional setting for hires sprites
  struct TagItem vcTags[] =
  {
    {VTAG_SPRITERESN_SET, SPRITERESN_70NS},
    {TAG_END}
  };

  m_pScreen = OpenScreenTags(NULL,
      SA_DisplayID, VP_MODE,
      SA_Depth, VP_DEPTH,
      SA_Width, VP_WIDTH,
      SA_Height, VP_HEIGHT,
      SA_ShowTitle, FALSE,
      SA_VideoControl, vcTags,
      TAG_END);

  if (m_pScreen == NULL)
  {
    return("Failed to open the screen.\n");
  }

  //
  // Load all needed colors
  //

  // Load the main 16 color palette of the background image
  LoadRGB32(&m_pScreen->ViewPort, m_PaletteBackgroundImg);

  // Load the arrow sprite colors: The start color register in the
  // color MakeClass(p depends on the sprite number we got
  int colReg = 16 + ((m_SpriteNumber & 0x06) << 1);

  // But the first of the 4 colors is unused (transparent)
  colReg++;

  int n = sizeof m_SpriteColors / ((sizeof m_SpriteColors[0]) * 3);
  for(int i = 0; i < n; i++)
  {
    int j = i * 3;

    SetRGB32(&m_pScreen->ViewPort, colReg + i, m_SpriteColors[j],
                                   m_SpriteColors[j + 1],
                                   m_SpriteColors[j + 2]);
  }

  //
  // Init the GELs system
  //
  m_pGelsInfo = setupGelSys(&m_pScreen->RastPort, 0x03);
  if (m_pGelsInfo == NULL)
  {
    return ("Failed to initialize the GELs system.\n");
  }

  //
  // Create the bobs for duck and hunter
  //

  NEWBOB newBob =
  {
    duckImages[0].pImageData,
    bitsToWords(duckImages[0].width), duckImages[0].height, duckImages[0].depth,
    15,                 // Plane pick, 01111 enables all 4 low planes
    0,                  // Plane on off (unused planes to turn on)
    SAVEBACK | OVERLAY, // duckImages flags
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

  newBob.nb_Image = hunterImages[0].pImageData;
  newBob.nb_WordWidth = bitsToWords(hunterImages[0].width);
  newBob.nb_LineHeight = hunterImages[0].height;
  newBob.nb_ImageDepth = hunterImages[0].depth;
  newBob.nb_X = 20;
  newBob.nb_Y = 216;

  m_pHunterBob = makeBob(&newBob);
  if (m_pHunterBob == NULL)
  {
    return ("Failed to create GELs bob for hunter.\n");
  }

  // Move the sprite to a visible position
//  MoveSprite(&m_pScreen->ViewPort, (struct SimpleSprite*) m_pArrowSprite, 100, 100);

  return NULL;
}

int cleanExit(char *pErrorMsg)
{
  if(m_SpriteNumber >= 0)
  {
    FreeSprite(m_SpriteNumber);
    m_SpriteNumber = -1;
  }

  if (m_pArrowSprite != NULL)
  {
    FreeSpriteData(m_pArrowSprite);
    m_pArrowSprite = NULL;
  }

  if (m_pHunterBob != NULL)
  {
    freeBob(m_pHunterBob, hunterImages[0].depth);
    m_pHunterBob = NULL;
  }

  if (m_pDuckBob != NULL)
  {
    freeBob(m_pDuckBob, duckImages[0].depth);
    m_pDuckBob = NULL;
  }

  if (m_pGelsInfo != NULL)
  {
    cleanupGelSys(m_pGelsInfo, &m_pScreen->RastPort);
    m_pGelsInfo = NULL;
  }

  if(m_pScreen != NULL)
  {
    CloseScreen(m_pScreen);
    m_pScreen = NULL;
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

