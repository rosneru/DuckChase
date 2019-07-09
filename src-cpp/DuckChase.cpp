#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/datatypes.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/gels.h>
#include <libraries/dos.h>
#include <datatypes/pictureclass.h>

#include "DatatypesPicture.h"
#include "animtools.h"
#include "animtools_proto.h"


VOID bobDrawGList(struct RastPort *rport, struct ViewPort *vport);
int do_Bob(struct Window *win);

long min (long a,long b)
{
  return (a < b ? a : b);
}

int main (void)
{
  int rc = RETURN_ERROR;
  struct Screen *pScreen;
  long imgx,imgy;
  long imgw,imgh;
  long winw,winh;
  long winx,winy;
  struct Window *pWindow;
  struct IntuiMessage *pMsg;

  WORD* pDuck1BobImageData = NULL;
  WORD* pDuck2BobImageData = NULL;

  if (pScreen = LockPubScreen ("StormScreen"))
//  if (pScreen = LockPubScreen (NULL))
  {
    DatatypesPicture dtPicBackground("/gfx/pic_background.iff");
    DatatypesPicture dtPicDuck1("/gfx/ente1_hires.iff");
    DatatypesPicture dtPicDuck2("/gfx/ente2_hires.iff");

    if ((dtPicBackground.Load(pScreen) == true)
     && (dtPicDuck1.Load(pScreen) == true)
     && (dtPicDuck2.Load(pScreen) == true))
    {
      struct BitMapHeader *bmhd = dtPicBackground.GetBitmapHeader();
      struct BitMap *bm = dtPicBackground.GetBitmap();

      int depth = dtPicDuck1.GetBitmapHeader()->bmh_Depth;
      int height = dtPicDuck1.GetBitmapHeader()->bmh_Height;
      int width = dtPicDuck1.GetBitmapHeader()->bmh_Width;

      int widthWords = ((width + 15) & -16) >> 4;

      struct BitMap *pBmDuck1 = dtPicDuck1.GetBitmap();

      // RASSIZE is a macro. It calculates how much memory is needed
      // for one bitplane of given size. It is fitting the width to
      // the next word size and calculates the number of bytes, then
      // multiplies them with the height.
      int s = RASSIZE(width, height);

      UBYTE* ptr = (UBYTE*)AllocMem(s * depth, MEMF_CHIP|MEMF_CLEAR);
      struct BitMap b;
      InitBitMap(&b, depth, width, height);

      for(int i = 0;i < depth;i++)
      {
        b.Planes[i] = ptr;
        ptr += s;
      }

      BltBitMap(pBmDuck1, 0, 0, &b, 0, 0, width, height, 0xC0, 0xFF, NULL);

      pDuck1BobImageData = (WORD*)b.Planes[0];

      NEWBOB myNewBob =
      {
        pDuck1BobImageData,   // Image data
        widthWords,           // Bob width (in number of 16-pixel-words)
        height,               // Bob height in lines
        depth,                // Image depth
        3,                    // Planes that get image data
        0,                    // Unused planes to turn on
        SAVEBACK | OVERLAY,   // Bog flags
        0,                    // DoubleBuffering. Set to '1' to activate.
        2,                    // Depth of the raster
        160,                  // Initial x position
        100,                  // Initial y position
        0,                    // Hit mask
        0,                    // Me mask
      };


      imgw = bmhd->bmh_Width;
      imgh = bmhd->bmh_Height;
      imgx = pScreen->WBorLeft;
      imgy = pScreen->WBorTop;

      winw = min(pScreen->Width, imgw);
      winw += pScreen->WBorLeft + pScreen->WBorRight;

      winh = min(pScreen->Height, imgh);
      winh += pScreen->WBorTop + pScreen->WBorBottom;


      winx = (pScreen->Width  - winw) >> 1;
      winy = (pScreen->Height - winh) >> 1;

      if (pWindow = OpenWindowTags (NULL,
          WA_CustomScreen,pScreen,
          WA_Left,winx,
          WA_Top,winy,
          WA_Width,winw,
          WA_Height,winh,
          WA_Flags,WFLG_ACTIVATE,
          WA_IDCMP,IDCMP_MOUSEBUTTONS|IDCMP_VANILLAKEY|IDCMP_INTUITICKS,
          TAG_END))
      {
        BltBitMapRastPort (bm, 0, 0, pWindow->RPort, imgx, imgy, imgw, imgh, 0xC0);
        BltBitMapRastPort (pBmDuck1, 0, 0, pWindow->RPort, imgx, imgy, 59, 21, 0xC0);
        BltBitMapRastPort (&b, 0, 0, pWindow->RPort, imgx + 70, imgy, 59, 21, 0xC0);

        struct Bob         *myBob;
        struct GelsInfo    *my_ginfo;

        if ((my_ginfo = setupGelSys(pWindow->RPort, 0x03)) != NULL)
        {

          if ((myBob = makeBob(&myNewBob)) != NULL)
          {
            AddBob(myBob, pWindow->RPort);
            bobDrawGList(pWindow->RPort, ViewPortAddress(pWindow));

            rc = RETURN_OK;
            bool bContinue = true;

            do
            {
              WaitPort (pWindow->UserPort);
              while (pMsg = (struct IntuiMessage *)GetMsg (pWindow->UserPort))
              {
                switch (pMsg->Class)
                {
                case IDCMP_VANILLAKEY:
                  if (pMsg->Code == 0x1b) /* Esc */
                  {
                    bContinue = false;
                  }
                  break;
                }

                myBob->BobVSprite->X = pMsg->MouseX + 20;
                myBob->BobVSprite->Y = pMsg->MouseY + 1;

                ReplyMsg ((struct Message *)pMsg);
              }

              // After getting a message, change the image data on the fly
//              myBob->BobVSprite->ImageData =
//                (myBob->BobVSprite->ImageData == pDuck1BobImageData) ?
 //                 pDuck2BobImageData : pDuck1BobImageData;

              InitMasks(myBob->BobVSprite);
              bobDrawGList(pWindow->RPort, ViewPortAddress(pWindow));
            }
            while (bContinue == true);

            RemBob(myBob);
            bobDrawGList(pWindow->RPort, ViewPortAddress(pWindow));
            freeBob(myBob, myNewBob.nb_RasDepth);
          }

          cleanupGelSys(my_ginfo,pWindow->RPort);
        }

        CloseWindow (pWindow);
      }

      //WaitBlit();
      FreeBitMap(&b);
    }

    UnlockPubScreen (NULL, pScreen);
  }

  PrintFault (IoErr(),NULL);
  return (rc);
}


/* Draw the Bobs into the RastPort. */
VOID bobDrawGList(struct RastPort *rport, struct ViewPort *vport)
{
  SortGList(rport);
  DrawGList(rport, vport);

  /* If the GelsList includes true VSprites, MrgCop() and LoadView() here */
  WaitTOF() ;
}
