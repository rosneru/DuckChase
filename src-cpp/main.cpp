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


#define GEL_SIZE 4

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
      WORD* pBob1ImageData = (WORD*)
        AllocVec(2 * 2 * GEL_SIZE * 2, MEMF_CHIP|MEMF_CLEAR);

      pBob1ImageData[0] = 0xffff;
      pBob1ImageData[1] = 0x0003;
      pBob1ImageData[2] = 0xfff0;
      pBob1ImageData[3] = 0x0003;
      pBob1ImageData[4] = 0xfff0;
      pBob1ImageData[5] = 0x0003;
      pBob1ImageData[6] = 0xffff;
      pBob1ImageData[7] = 0x0003;
      pBob1ImageData[8] = 0x3fff;
      pBob1ImageData[9] = 0xfffc;
      pBob1ImageData[10] = 0x3ff0;
      pBob1ImageData[11] = 0x0ffc;
      pBob1ImageData[12] = 0x3ff0;
      pBob1ImageData[13] = 0x0ffc;
      pBob1ImageData[14] = 0x3fff;
      pBob1ImageData[15] = 0xfffc;

      NEWBOB myNewBob =
      {
        pBob1ImageData,     // Image data
        2,                  // Bob width (in number of 16-pixel-words)
        GEL_SIZE,           // Bob height in lines
        2,                  // Image depth
        3,                  // Planes that get image data (TODO whats this??)
        0,                  // Unused planes to turn on
        SAVEBACK | OVERLAY, // Bog flags
        0,                  // DoubleBuffering. Set to '1' to activate.
        3,                  // Depth of the raster
        25,                 // Initial x position
        25,                 // Initial y position
        0,                  // Hit mask
        0,                  // Me mask
      };

      struct Bob         *myBob;
      struct GelsInfo    *my_ginfo;

      if ((my_ginfo = setupGelSys(pWindow->RPort, 0x03)) != NULL)
      {

        if ((myBob = makeBob(&myNewBob)) != NULL)
        {
          AddBob(myBob, pWindow->RPort);
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

              myBob->BobVSprite->X = pMsg->MouseX + 20;
              myBob->BobVSprite->Y = pMsg->MouseY + 1;

              ReplyMsg ((struct Message *)pMsg);
            }

            InitMasks(myBob->BobVSprite);
            bobDrawGList(pWindow->RPort, ViewPortAddress(pWindow));
          }
          while (bContinue);

          RemBob(myBob);
          bobDrawGList(pWindow->RPort, ViewPortAddress(pWindow));
          freeBob(myBob, myNewBob.nb_RasDepth);
        }

        cleanupGelSys(my_ginfo, pWindow->RPort);
      }

      FreeVec(pBob1ImageData);
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
