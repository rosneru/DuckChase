#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <exec/memory.h>
#include <devices/timer.h>
#include <graphics/displayinfo.h>
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

void drawGels();

struct DimensionInfo dimsinfo;

struct View view, *oldview=NULL;
struct ViewPort viewPort;

struct BitMap bitMap1;
//struct BitMap bitMap2;
struct RastPort rastPort1;
//struct RastPort rastPort2;
//struct RastPort *rastPort;

struct RasInfo rasInfo;

//struct cprlist *LOCpr1;
//struct cprlist *SHCpr1;
//struct cprlist *LOCpr2;
//struct cprlist *SHCpr2;

LONG oscan_height;

//WORD ToggleFrame = 0;

int main(int argc, char **argv)
{
  SetJoyPortAttrs(1,
                  SJA_Type, SJA_TYPE_AUTOSENSE,
                  TAG_END);

  SystemControl(SCON_TakeOverSys, TRUE,
                TAG_END);





  struct DimensionInfo dimsinfo;
  GetDisplayInfoData(FindDisplayInfo(HIRES_KEY),
                    (UBYTE *)&dimsinfo,
                    sizeof(struct DimensionInfo),
                    DTAG_DIMS,
                    NULL);

  oscan_height = dimsinfo.MaxOScan.MaxY
    - dimsinfo.MaxOScan.MinY + 1;

  LoadView(NULL);
  WaitTOF();
  WaitTOF();

  oldview = GfxBase->ActiView;

  InitView(&view);

  InitBitMap(&bitMap1, 3, 640, 256);
//  InitBitMap(&bitMap2, 3, 640, 256);

  for (int depth=0; depth<3; depth++)
  {
    bitMap1.Planes[depth] = (PLANEPTR)AllocRaster(640, 256);
//    bitMap2.Planes[depth] = (PLANEPTR)AllocRaster(640, 256);
  }

  InitRastPort(&rastPort1);
  rastPort1.BitMap = &bitMap1;
  SetRast(&rastPort1, 0);

//  InitRastPort(&rastPort2);
//  rastPort2.BitMap = &bitMap2;
//  SetRast(&rastPort2, 0);

  rasInfo.BitMap = &bitMap1;
  rasInfo.RxOffset = 0;
  rasInfo.RyOffset = 0;
  rasInfo.Next = NULL;

  InitVPort(&viewPort);
  view.ViewPort = &viewPort;
  viewPort.RasInfo = &rasInfo;
  viewPort.DWidth = 640;
  viewPort.DHeight = 256;
  viewPort.Modes = HIRES;

  MakeVPort(&view, &viewPort);
  MrgCop(&view);

/*
  LOCpr1 = view.LOFCprList;
  SHCpr1 = view.SHFCprList;
  view.LOFCprList = 0;
  view.SHFCprList = 0;

  rasInfo.BitMap = &bitMap2;

  MakeVPort(&view, &viewPort);
  MrgCop(&view);

  LOCpr2 = view.LOFCprList;
  SHCpr2 = view.SHFCprList;

  LoadView(&view);
*/

  LoadView(&view);

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
    SetRGB4(&viewPort, i, colArr[i][0], colArr[i][1], colArr[i][2]);
  }

  //
  // Loading background image
  //
  Picture picBackgr;
  bool bOk = picBackgr.LoadFromRawFile("/gfx/background_hires.raw",
                                       640, 256, 3);

  if(bOk)
  {
    BltBitMapRastPort(picBackgr.GetBitMap(), 0, 0, &rastPort1,
                      0, 0, 640, 256, 0xC0);
/*
    BltBitMapRastPort(picBackgr.GetBitMap(), 0, 0, &rastPort2,
                      0, 0, 640, 256, 0xC0);
*/
  }


  struct GelsInfo* pGelsInfo;

  if ((pGelsInfo = setupGelSys(&rastPort1, 0x03)) != NULL)
  {
    GelsBob bobDuck(3, 59, 21, 3);
    bobDuck.LoadImgFromRawFile("/gfx/ente1_hires.raw");
    bobDuck.LoadImgFromRawFile("/gfx/ente2_hires.raw");

    GelsBob bobHunter(3, 16, 22, 3);
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

      AddBob(pBobDuck, &rastPort1);
      AddBob(pBobHunter, &rastPort1);

      drawGels();

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
        drawGels();

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

          SetBPen(&rastPort1, 0);
          EraseRect(&rastPort1, 52, 62, 130, 72);

          Move(&rastPort1, 50, 70);
          Text(&rastPort1, pFpsBuf, strlength(pFpsBuf));
        }
      }
      while (bContinue);

      RemBob(pBobHunter);
      RemBob(pBobDuck);

      drawGels();
    }

    cleanupGelSys(pGelsInfo, &rastPort1);
  }

  WaitTOF();
  WaitTOF();
  LoadView(oldview);
  WaitTOF();
/*
  FreeCprList(LOCpr1);
  FreeCprList(LOCpr2);
  FreeCprList(SHCpr1);
  FreeCprList(SHCpr2);
*/
  FreeCprList(view.LOFCprList);
  FreeVPortCopLists(&viewPort);

  for(int depth=0; depth<3; depth++)
  {
    if (bitMap1.Planes[depth])
    {
      FreeRaster(bitMap1.Planes[depth], 640, 256);
    }
/*
    if (bitMap2.Planes[depth])
    {
      FreeRaster(bitMap2.Planes[depth], 640, 256);
    }
*/
  }
  SystemControl(SCON_TakeOverSys, FALSE,
                TAG_END);

}

/**
 * Draw the Bobs into the RastPort.
 */
void drawGels()
{
  SortGList(&rastPort1);
  DrawGList(&rastPort1, &viewPort);


  // If the GelsList includes true VSprites, MrgCop() and LoadView()
  // here
  WaitTOF();

/*
  if(ToggleFrame == 0)
  {
    view.LOFCprList = LOCpr2;
    view.SHFCprList = SHCpr2;
    rastPort = &rastPort2;
  }
  else
  {
    view.LOFCprList = LOCpr1;
    view.SHFCprList = SHCpr1;
    rastPort = &rastPort1;
  }
*/
  LoadView(&view);

//  ToggleFrame ^=1;
}
