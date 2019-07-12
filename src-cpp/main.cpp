#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <exec/memory.h>

#include <graphics/modeid.h>

#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

#include "animtools.h"
#include "animtools_proto.h"
#include "DatatypesPicture.h"
#include "GelsBob.h"


void bobDrawGList(struct RastPort *rport, struct ViewPort *vport);


int main(int argc, char **argv)
{
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
      DatatypesPicture dtPic("/gfx/pic_background.iff");
      if(dtPic.Load(pScreen) == true)
      {
        BltBitMapRastPort(dtPic.GetBitmap(), 0, 0, pWindow->RPort,
                          0, 0, 640, 256, 0xC0);
      }



      struct GelsInfo* pGelsInfo;

      if ((pGelsInfo = setupGelSys(pWindow->RPort, 0x03)) != NULL)
      {
        GelsBob gelsBob1(pScreen, pWindow, 3, 59, 21, 3);

        if(gelsBob1.CreateFromRawFile("/gfx/ente1_hires.raw") == true)
        {

          struct Bob* pBob1 = gelsBob1.Get();

          AddBob(pBob1, pWindow->RPort);
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

              pBob1->BobVSprite->X = pMsg->MouseX + 20;
              pBob1->BobVSprite->Y = pMsg->MouseY + 1;

              ReplyMsg ((struct Message *)pMsg);
            }

            InitMasks(pBob1->BobVSprite);
            bobDrawGList(pWindow->RPort, pViewPort);
          }
          while (bContinue);

          RemBob(pBob1);
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
