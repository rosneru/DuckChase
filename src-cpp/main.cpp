#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <exec/memory.h>

#include <graphics/modeid.h>

#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <libraries/lowlevel.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/lowlevel_protos.h>

#include "animtools.h"
#include "animtools_proto.h"
#include "DatatypePic.h"
#include "GelsBob.h"


void bobDrawGList(struct RastPort *rport,
                  struct ViewPort *vport);


int main(int argc, char **argv)
{
  SetJoyPortAttrs(1,
                  SJA_Type, SJA_TYPE_AUTOSENSE,
                  TAG_END);

  UWORD pens[] = { ~0 };

  struct Screen* pScreen = OpenScreenTags(NULL,
    SA_Pens, pens,
    SA_Depth, 3,
    SA_Top, 0,
    SA_Left, 0,
    SA_Width, 640,
    SA_Height, 256,
    SA_DisplayID, PAL_MONITOR_ID|HIRES_KEY,
    TAG_END);

  if (pScreen != NULL)
  {
    struct Window* pWindow = OpenWindowTags (NULL,
        WA_CustomScreen, pScreen,
        WA_Left, 0,
        WA_Top, 0,
        WA_Width, 640,
        WA_Height,256,
        WA_Flags, WFLG_ACTIVATE,
        WA_IDCMP, IDCMP_VANILLAKEY|IDCMP_INTUITICKS,
        TAG_END);

    if(pWindow != NULL)
    {
      struct ViewPort* pViewPort = ViewPortAddress(pWindow);

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
        SetRGB4(pViewPort, i, colArr[i][0], colArr[i][1], colArr[i][2]);
      }

      //
      // Loading background image
      //
      DatatypePic picBackgr("/gfx/background_hires.iff");
      if(picBackgr.Load(pScreen) == true)
      {
        BltBitMapRastPort(picBackgr.GetBitmap(), 0, 0, pWindow->RPort,
                          0, 0, 640, 256, 0xC0);
      }



      struct GelsInfo* pGelsInfo;

      if ((pGelsInfo = setupGelSys(pWindow->RPort, 0x03)) != NULL)
      {
        GelsBob bobDuck(pScreen, pWindow, 3, 59, 21, 3);
        bobDuck.LoadImgFromRawFile("/gfx/ente1_hires.raw");
        bobDuck.LoadImgFromRawFile("/gfx/ente2_hires.raw");

        GelsBob bobHunter(pScreen, pWindow, 3, 16, 22, 3);
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

          AddBob(pBobDuck, pWindow->RPort);
          AddBob(pBobHunter, pWindow->RPort);

          bobDrawGList(pWindow->RPort, ViewPortAddress(pWindow));

          struct IntuiMessage* pMsg;
          bool bContinue = true;

          do
          {
            WaitPort (pWindow->UserPort);
            while (pMsg = (struct IntuiMessage *)GetMsg (pWindow->UserPort))
            {
              switch (pMsg->Class)
              {
                case IDCMP_VANILLAKEY:
                {
                  if (pMsg->Code == 0x1b) // ESC key
                  {
                    bContinue = false;
                  }
                  break;
                }
              }

/*
              pBobHunter->BobVSprite->X = pMsg->MouseX + 20;
              pBobHunter->BobVSprite->Y = pMsg->MouseY + 1;
*/
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

              ReplyMsg ((struct Message *)pMsg);
            }

            bobDuck.NextImage();
            InitMasks(pBobDuck->BobVSprite);
            bobDrawGList(pWindow->RPort, pViewPort);
          }
          while (bContinue);

          RemBob(pBobHunter);
          RemBob(pBobDuck);

          bobDrawGList(pWindow->RPort, pViewPort);
        }

        cleanupGelSys(pGelsInfo, pWindow->RPort);
      }

      CloseWindow(pWindow);
    }

    CloseScreen(pScreen);
  }
}

/**
 * Draw the Bobs into the RastPort.
 */
void bobDrawGList(struct RastPort *rport, struct ViewPort *vport)
{
  SortGList(rport);
  DrawGList(rport, vport);

  // If the GelsList includes true VSprites, MrgCop() and LoadView()
  // here
  WaitTOF() ;
}
