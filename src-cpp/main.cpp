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
      struct GelsInfo* pGelsInfo;

      if ((pGelsInfo = setupGelSys(pWindow->RPort, 0x03)) != NULL)
      {
        GelsBob gelsBob1(pScreen, 3);

        if(gelsBob1.CreateFromRawFile("/gfx/rkrm_bob_data_1.raw",
                                      32,
                                      4,
                                      2) == true)
        {

          struct Bob* pBob1 = gelsBob1.GetBob();

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
            bobDrawGList(pWindow->RPort, ViewPortAddress(pWindow));
          }
          while (bContinue);

          RemBob(pBob1);
          bobDrawGList(pWindow->RPort, ViewPortAddress(pWindow));
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
