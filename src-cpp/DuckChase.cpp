#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/datatypes.h>
#include <datatypes/pictureclass.h>

long min (long a,long b)
{
  return (a < b ? a : b);
}

int main (void)
{
  static struct {
    char *file;
    char *pubscreen;
  } args = {NULL};

  int rc = RETURN_ERROR;
  struct Screen *scr;
  Object *o;
  struct BitMapHeader *bmhd;
  struct BitMap *bm;
  long imgx,imgy;
  long imgw,imgh;
  long winw,winh;
  long winx,winy;
  struct Window *win;
  BOOL cont;
  struct IntuiMessage *mess;

  const char* pPubScreenName = "StormScreen";
  const char* pBgImgName = "/gfx/pic_background.iff";

  if (scr = LockPubScreen (pPubScreenName))
  {
    if (o = NewDTObject ((void*)pBgImgName,
        DTA_GroupID,GID_PICTURE,
        PDTA_Remap,TRUE,
        PDTA_Screen,scr,
        TAG_END))

    {
      DoDTMethod (o,NULL,NULL,DTM_PROCLAYOUT,NULL,TRUE);

      GetDTAttrs (o,
        PDTA_BitMapHeader,&bmhd,
        PDTA_DestBitMap,&bm,
        TAG_END);

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
        BltBitMapRastPort (bm,0,0,win->RPort,imgx,imgy,imgw,imgh,0xC0);

        rc = RETURN_OK;

        cont = TRUE;
        do
        {
          WaitPort (win->UserPort);
          while (mess = (struct IntuiMessage *)GetMsg (win->UserPort))
          {
            switch (mess->Class)
            {
            case IDCMP_MOUSEBUTTONS:
              if (mess->Code == IECODE_LBUTTON)
                cont = FALSE;
              break;
            case IDCMP_VANILLAKEY:
              if (mess->Code == 0x1b) /* Esc */
                cont = FALSE;
              break;
            }
            ReplyMsg ((struct Message *)mess);
          }
        }
        while (cont);

        CloseWindow (win);
      }

      DisposeDTObject (o);
    }

    UnlockPubScreen (NULL,scr);
  }

  PrintFault (IoErr(),NULL);
  return (rc);
}
