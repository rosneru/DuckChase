#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <exec/memory.h>
#include <devices/timer.h>
#include <graphics/modeid.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <libraries/lowlevel.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/lowlevel_protos.h>


#include "animtools.h"
#include "animtools_proto.h"

#include "stdiostring.h"

#include "Picture.h"
#include "GelsBob.h"

extern struct GfxBase* GfxBase;

void drawGels(struct Screen* p_pScreen);

// Double buffering
struct BitMap *MyBitMapPtrs[2] =
{
  NULL, NULL
};

WORD ToggleFrame = 0;

int main(int argc, char **argv)
{
  SetJoyPortAttrs(1,
                  SJA_Type, SJA_TYPE_AUTOSENSE,
                  TAG_END);

  UWORD pens[] = { ~0 };

  // Double buffering
  for(int j=0; j < 2; j++)
  {
    if ((MyBitMapPtrs[j] = (struct BitMap *)
      AllocMem(sizeof(struct BitMap), MEMF_CHIP)) == NULL)
    {
      return 0;
    }

    InitBitMap(MyBitMapPtrs[j], 3, 640, 256);

    for(int i = 0; i < 3; i++)
    {
      if ((MyBitMapPtrs[j]->Planes[i] = (PLANEPTR)
        AllocRaster(640, 256)) == NULL)
      {
        return 0;
      }

      BltClear(MyBitMapPtrs[j]->Planes[i], (640 / 8) * 256, 1);
    }
  }

  struct Screen* pScreen = OpenScreenTags(NULL,
    SA_Pens, pens,
    SA_Depth, 3,
    SA_Top, 0,
    SA_Left, 0,
    SA_Width, 640,
    SA_Height, 256,
    SA_DisplayID, PAL_MONITOR_ID|HIRES_KEY,
    SA_Quiet, TRUE,
    SA_Exclusive, TRUE,
    SA_Interleaved, TRUE,
    SA_Type, CUSTOMSCREEN,
    SA_BitMap, MyBitMapPtrs[0],
    TAG_END);

  if (pScreen != NULL)
  {
    //
    // Setting the used color table (extracted from pic wit BtoC32)
    //
    UBYTE colArr[8][3] =
    {
      {0xA,0xA,0xA}, {0x0,0x0,0x0}, {0xF,0xF,0xF}, {0x6,0x8,0xB},
      {0x5,0xA,0x3}, {0xE,0xB,0x0}, {0xB,0x5,0x2}, {0xF,0x8,0x0}
    };

    for(int i = 0; i < 8; i++)
    {
      SetRGB4(&pScreen->ViewPort, i,
              colArr[i][0], colArr[i][1], colArr[i][2]);
    }

    //
    // Loading background image
    //
    Picture picBackgr;
    bool bOk = picBackgr.LoadFromRawFile("/gfx/background_hires.raw",
                                         640, 256, 3);

    if(bOk)
    {
      BltBitMapRastPort(picBackgr.GetBitMap(), 0, 0, &pScreen->RastPort,
                        0, 0, 640, 256, 0xC0);
    }



    struct GelsInfo* pGelsInfo;

    if ((pGelsInfo = setupGelSys(&pScreen->RastPort, 0x03)) != NULL)
    {
      GelsBob bobDuck(pScreen, 59, 21, 3);
      bobDuck.LoadImgFromRawFile("/gfx/ente1_hires.raw");
      bobDuck.LoadImgFromRawFile("/gfx/ente2_hires.raw");

      GelsBob bobHunter(pScreen, 16, 22, 3);
      bobHunter.LoadImgFromRawFile("/gfx/jaeger1_hires.raw");
      bobHunter.LoadImgFromRawFile("/gfx/jaeger2_hires.raw");

      struct Bob* pBobDuck = bobDuck.Get();
      struct Bob* pBobHunter = bobHunter.Get();

      if((pBobDuck != NULL) && (pBobHunter != NULL))
      {
        pBobDuck->BobVSprite->X = 200;
        pBobDuck->BobVSprite->Y = 40;

        pBobHunter->BobVSprite->X = 20;
        pBobHunter->BobVSprite->Y = 220;

        AddBob(pBobDuck, &pScreen->RastPort);
        AddBob(pBobHunter, &pScreen->RastPort);

        drawGels(pScreen);

        bool bContinue = true;
        char pFpsBuf[] = {"FPS: __________"};
        char* pFpsNumberStart = pFpsBuf + 5;

        struct EClockVal eClockVal;
        eClockVal.ev_hi = 0;
        eClockVal.ev_lo = 0;

        ULONG elapsed = ElapsedTime(&eClockVal);

        do
        {
          ULONG key = GetKey();
          if((key & 0x00ff) == 0x45) // RAW code ESC key
          {
            bContinue = false;
          }

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

          pBobDuck->BobVSprite->X -= 4;
          if(pBobDuck->BobVSprite->X < -40)
          {
            pBobDuck->BobVSprite->X = 650;
          }

          bobDuck.NextImage();
          InitMasks(pBobDuck->BobVSprite);
          drawGels(pScreen);

          //
          // Display the FPS value
          //
          elapsed = ElapsedTime(&eClockVal);
          elapsed &= 0xffff;

          if(elapsed >= 0)
          {
            // Calculatin fps and writing it to the string buf
            short fps = 65536 / elapsed;
            itoa(fps, pFpsNumberStart, 10);

            SetBPen(&pScreen->RastPort, 0);
            EraseRect(&pScreen->RastPort, 52, 62, 130, 72);

            Move(&pScreen->RastPort, 50, 70);
            Text(&pScreen->RastPort, pFpsBuf, strlength(pFpsBuf));
          }
        }
        while (bContinue);

        RemBob(pBobHunter);
        RemBob(pBobDuck);

        drawGels(pScreen);
      }

      cleanupGelSys(pGelsInfo, &pScreen->RastPort);
    }

    CloseScreen(pScreen);
  }
}

/**
 * Draw the Bobs into the RastPort.
 */
void drawGels(struct Screen* p_pScreen)
{
  SortGList(&p_pScreen->RastPort);
  DrawGList(&p_pScreen->RastPort, &p_pScreen->ViewPort);

  // Double buffering
  p_pScreen->ViewPort.RasInfo->BitMap = MyBitMapPtrs[ToggleFrame];

  // If the GelsList includes true VSprites, MrgCop() and LoadView()
  // here
  WaitTOF();

  // Tell intuition to do its stuff
  MakeScreen(p_pScreen);

  LoadView(GfxBase->ActiView);

  // Double buffering
  ToggleFrame ^=1;

  // Double buffering
  p_pScreen->RastPort.BitMap = MyBitMapPtrs[ToggleFrame];

}
