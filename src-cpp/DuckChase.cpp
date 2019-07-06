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
#include <stdlib.h>
#include <datatypes/pictureclass.h>

#include "DatatypesPicture.h"
#include "animtools.h"
#include "animtools_proto.h"


VOID bobDrawGList(struct RastPort *rport, struct ViewPort *vport);
int do_Bob(struct Window *win);

#define GEL_SIZE 21 // Number of lines of the bob

NEWBOB myNewBob =                     /* Data for the new bob structure defined in animtools.h */
        {                             /* Initial image, WORD width, line height                */
        NULL, 2,  GEL_SIZE,      /* Image depth, plane pick, plane on off, VSprite flags  */
        2, 3, 0, SAVEBACK | OVERLAY,  /* dbuf (0=false), raster depth, x,y position, hit mask, */
        0, 2, 160, 100,  0,0,         /* me mask                                               */
        };



long min (long a,long b)
{
  return (a < b ? a : b);
}

int main (void)
{
  int rc = RETURN_ERROR;
  struct Screen *scr;
  long imgx,imgy;
  long imgw,imgh;
  long winw,winh;
  long winx,winy;
  struct Window *win;
  BOOL cont;
  struct IntuiMessage *pMsg;

  const char* pPubScreenName = "StormScreen";
  const char* pBgPicPath = "/gfx/pic_background.iff";
  const char* pDuck1PicPath = "/gfx/ente1_brush.iff";
  const char* pDuck2PicPath = "/gfx/ente2_brush.iff";

  if (scr = LockPubScreen (pPubScreenName))
  {
    DatatypesPicture bgPic(pBgPicPath);
    DatatypesPicture duck1Pic(pDuck1PicPath);
    DatatypesPicture duck2Pic(pDuck2PicPath);

    if ((bgPic.Load(scr) == true)
     && (duck1Pic.Load(scr) == true)
     && (duck2Pic.Load(scr) == true))
    {
      struct BitMapHeader *bmhd = bgPic.GetBitmapHeader();
      struct BitMap *bm = bgPic.GetBitmap();

      imgw = bmhd->bmh_Width;
      imgh = bmhd->bmh_Height;
      imgx = scr->WBorLeft;
      imgy = scr->WBorTop;

      winw = min(scr->Width, imgw);
      winw += scr->WBorLeft + scr->WBorRight;

      winh = min(scr->Height, imgh);
      winh += scr->WBorTop + scr->WBorBottom;


      winx = (scr->Width  - winw) >> 1;
      winy = (scr->Height - winh) >> 1;

      if (win = OpenWindowTags (NULL,
          WA_CustomScreen,scr,
          WA_Left,winx,
          WA_Top,winy,
          WA_Width,winw,
          WA_Height,winh,
          WA_Flags,WFLG_ACTIVATE,
          WA_IDCMP,IDCMP_MOUSEBUTTONS|IDCMP_VANILLAKEY,
          TAG_END))
      {
        BltBitMapRastPort (bm, 0, 0, win->RPort, imgx, imgy, imgw, imgh, 0xC0);

        myNewBob.nb_Image = (WORD*) duck1Pic.GetBitmap();
        struct Bob         *myBob;
        struct GelsInfo    *my_ginfo;

        if ((my_ginfo = setupGelSys(win->RPort, 0x03)) != NULL)
        {

          if ((myBob = makeBob(&myNewBob)) != NULL)
          {
            AddBob(myBob, win->RPort);
            bobDrawGList(win->RPort, ViewPortAddress(win));

            rc = RETURN_OK;
            cont = TRUE;

            do
            {
              WaitPort (win->UserPort);
              while (pMsg = (struct IntuiMessage *)GetMsg (win->UserPort))
              {
                switch (pMsg->Class)
                {
//                case IDCMP_MOUSEBUTTONS:
//                  if (pMsg->Code == IECODE_LBUTTON)
//                    cont = FALSE;
//                  break;
                case IDCMP_VANILLAKEY:
                  if (pMsg->Code == 0x1b) /* Esc */
                    cont = FALSE;
                  break;
                }

                myBob->BobVSprite->X = pMsg->MouseX + 20;
                myBob->BobVSprite->Y = pMsg->MouseY + 1;

                ReplyMsg ((struct Message *)pMsg);
              }

              // After getting a message, change the image data on the fly
              myBob->BobVSprite->ImageData = (myBob->BobVSprite->ImageData == (WORD*)duck1Pic.GetBitmap())
                ? (WORD*)duck2Pic.GetBitmap() : (WORD*)duck1Pic.GetBitmap();

              InitMasks(myBob->BobVSprite);
              bobDrawGList(win->RPort, ViewPortAddress(win));
            }
            while (cont);

            RemBob(myBob);
            bobDrawGList(win->RPort, ViewPortAddress(win));
            freeBob(myBob, myNewBob.nb_RasDepth);
          }

          cleanupGelSys(my_ginfo,win->RPort);
        }

        CloseWindow (win);
      }
    }

    UnlockPubScreen (NULL,scr);
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
