/*
 * doublebuffer.c - shows double-buffering, attached screens, menu lending
 *
 * (c) Copyright 1992-1999 Amiga, Inc.  All rights reserved.
 *
 * This software is provided as-is and is subject to change; no warranties
 * are made.  All use is at your own risk.  No liability or responsibility
 * is assumed.
 *
 * Example shows use of V39 double-buffering functions to achieve
 * 60 frame-per-second animation.  Also shows use of menus in
 * double-buffered screens, and the use of attached screens with
 * menu-lending to achieve the appearance of slider gadgets in
 * double-buffered screens.
 *
 */

 /*----------------------------------------------------------------------*/

#include <exec/types.h>
#include <graphics/videocontrol.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>

#include <stdlib.h>
#include <stdio.h>

#include "AnimFrameTool.h"

#define CONTROLSC_TOP    191
#define SC_ID      HIRES_KEY

#define OK_REDRAW 1  // Buffer fully detached, ready for redraw
#define OK_SWAPIN 2  // Buffer redrawn, ready for swap-in


/* Some constants to handle the rendering of the animated face */
#define BM_WIDTH  120
#define BM_HEIGHT  60
#define BM_DEPTH  2


LONG prevx[2] =
{
    50, 50,
};

LONG prevy[2] =
{
    50, 50,
};


struct BitMap *face = NULL;
LONG x, y, xstep, xdir, ystep, ydir;


struct TextAttr Topaz80 =
{
  "topaz.font",               // Name
  8,                          // YSize
  FS_NORMAL,                  // Style
  FPF_ROMFONT | FPF_DESIGNED, // Flags
};



AnimFrameTool::AnimFrameTool()
  : m_pCanvasScreen(NULL),
    m_pControlScreen(NULL),
    m_pCanvasWindow(NULL),
    m_pControlWindow(NULL),
    m_pGadgetList(NULL),
    m_pGadgetSlideHorizontal(NULL),
    m_pGadgetSlideVertical(NULL),
    m_pMenu(NULL),
    m_pVisualInfoCanvas(NULL),
    m_pVisualInfoControl(NULL),
    m_pDBufPort(NULL),
    m_pUserPort(NULL),
    m_pScreenBuffers(),
    m_RastPorts(),
    m_BufCurrent(0),
    m_BufNextdraw(1),
    m_BufNextswap(1),
    m_Count(0)
{
  if (!(m_pDBufPort = CreateMsgPort()))
  {
    cleanup();
    throw "Failed to create port";
  }

  if (!(m_pUserPort = CreateMsgPort()))
  {
    cleanup();
    throw "Failed to create port";
  }


  struct TagItem vctags[] =
  {
      VTAG_BORDERSPRITE_SET, TRUE,
      TAG_DONE, 0,
  };

  UWORD pens[] =
  {
      0, // DETAILPEN
      1, // BLOCKPEN
      1, // TEXTPEN
      2, // SHINEPEN
      1, // SHADOWPEN
      3, // FILLPEN
      1, // FILLTEXTPEN
      0, // BACKGROUNDPEN
      2, // HIGHLIGHTTEXTPEN
      1, // BARDETAILPEN
      2, // BARBLOCKPEN
      1, // BARTRIMPEN

      (UWORD)~0,
  };

  struct NewMenu demomenu[] =
  {
    { NM_TITLE, "Project",             0 , 0, 0, 0, },
    {  NM_ITEM, "Run",                "R", 0, 0, (APTR)MID_Run, },
    {  NM_ITEM, "Step",               "S", 0, 0, (APTR)MID_Step, },
    {  NM_ITEM, NM_BARLABEL,           0 , 0, 0, 0, },
    {  NM_ITEM, "Slower Horizontal",  "1", 0, 0, (APTR)MID_HSlow, },
    {  NM_ITEM, "Faster Horizontal",  "2", 0, 0, (APTR)MID_HFast, },
    {  NM_ITEM, "Slower Vertical",    "3", 0, 0, (APTR)MID_VSlow, },
    {  NM_ITEM, "Faster Vertical",    "4", 0, 0, (APTR)MID_VFast, },
    {  NM_ITEM, NM_BARLABEL,           0 , 0, 0, 0, },
    {  NM_ITEM, "Quit",               "Q", 0, 0, (APTR)MID_Quit, },
    { NM_END,   0,                     0 , 0, 0, 0, },
  };

  if (!(m_pCanvasScreen = OpenScreenTags(NULL,
    SA_DisplayID, SC_ID,
    SA_Overscan, OSCAN_TEXT,
    SA_Depth, 2,
    SA_AutoScroll, 1,
    SA_Pens, pens,
    SA_ShowTitle, TRUE,
    SA_Title, "Intuition double-buffering example",
    SA_VideoControl, vctags,
    SA_Font, &Topaz80,
    TAG_DONE)))
  {
    cleanup();
    throw "Couldn't open screen";
  }

  if (!(m_pVisualInfoCanvas = GetVisualInfo(m_pCanvasScreen,
    TAG_DONE)))
  {
    cleanup();
    throw "Couldn't get VisualInfo";
  }

  if (!(m_pCanvasWindow = OpenWindowTags(NULL,
    WA_NoCareRefresh, TRUE,
    WA_Activate, TRUE,
    WA_Borderless, TRUE,
    WA_Backdrop, TRUE,
    WA_CustomScreen, m_pCanvasScreen,
    WA_NewLookMenus, TRUE,
    TAG_DONE)))
  {
    cleanup();
    throw "Couldn't open window";
  }
  m_pCanvasWindow->UserPort = m_pUserPort;

  ModifyIDCMP(m_pCanvasWindow, IDCMP_MENUPICK | IDCMP_VANILLAKEY);

  if (!(m_pControlScreen = OpenScreenTags(NULL,
    SA_DisplayID, SC_ID,
    SA_Overscan, OSCAN_TEXT,
    SA_Depth, 2,
    SA_Pens, pens,
    SA_Top, CONTROLSC_TOP,
    SA_Height, 28,
    SA_Parent, m_pCanvasScreen,
    SA_ShowTitle, FALSE,
    SA_Draggable, FALSE,
    SA_VideoControl, vctags,
    SA_Quiet, TRUE,
    SA_Font, &Topaz80,
    TAG_DONE)))
  {
    cleanup();
    throw "Couldn't open screen";
  }

  if (!(m_pVisualInfoControl = GetVisualInfo(m_pControlScreen,
    TAG_DONE)))
  {
    cleanup();
    throw "Couldn't get VisualInfo\n";
  }

  if (!(m_pMenu = CreateMenus(demomenu,
    TAG_DONE)))
  {
    cleanup();
    throw "Couldn't create menus";
  }

  if (!LayoutMenus(m_pMenu, m_pVisualInfoCanvas,
    GTMN_NewLookMenus, TRUE,
    TAG_DONE))
  {
    cleanup();
    throw "Couldn't layout menus";
  }

  if (!createAllGadgets(&m_pGadgetList, m_pVisualInfoControl))
  {
    cleanup();
    throw "Couldn't create gadgets";
  }

  /* A borderless backdrop window so we can get input */
  if (!(m_pControlWindow = OpenWindowTags(NULL,
    WA_NoCareRefresh, TRUE,
    WA_Activate, TRUE,
    WA_Borderless, TRUE,
    WA_Backdrop, TRUE,
    WA_CustomScreen, m_pControlScreen,
    WA_NewLookMenus, TRUE,
    WA_Gadgets, m_pGadgetList,
    TAG_DONE)))
  {
    cleanup();
    throw "Couldn't open window";
  }

  m_pControlWindow->UserPort = m_pUserPort;
  ModifyIDCMP(m_pControlWindow, SLIDERIDCMP | IDCMP_MENUPICK | IDCMP_VANILLAKEY);

  GT_RefreshWindow(m_pControlWindow, NULL);
  SetMenuStrip(m_pCanvasWindow, m_pMenu);
  LendMenus(m_pControlWindow, m_pCanvasWindow);

  if (!(m_pScreenBuffers[0] = AllocScreenBuffer(m_pCanvasScreen, NULL, SB_SCREEN_BITMAP)))
  {
    cleanup();
    throw "Couldn't allocate ScreenBuffer 1";
  }

  if (!(m_pScreenBuffers[1] = AllocScreenBuffer(m_pCanvasScreen, NULL, SB_COPY_BITMAP)))
  {
    cleanup();
    throw "Couldn't allocate ScreenBuffer 2";
  }

  /* Let's use the UserData to store the buffer number, for
   * easy identification when the message comes back.
   */
  m_pScreenBuffers[0]->sb_DBufInfo->dbi_UserData1 = (APTR)(0);
  m_pScreenBuffers[1]->sb_DBufInfo->dbi_UserData1 = (APTR)(1);
  m_Status[0] = OK_REDRAW;
  m_Status[1] = OK_REDRAW;

  if (!(face = makeImageBM()))
  {
    cleanup();
    throw "Couldn't allocate image bitmap";
  }

  InitRastPort(&m_RastPorts[0]);
  InitRastPort(&m_RastPorts[1]);
  m_RastPorts[0].BitMap = m_pScreenBuffers[0]->sb_BitMap;
  m_RastPorts[1].BitMap = m_pScreenBuffers[1]->sb_BitMap;

  x = 50;
  y = 70;
  xstep = 1;
  xdir = 1;
  ystep = 1;
  ydir = -1;
}


AnimFrameTool::~AnimFrameTool()
{
  cleanup();
}

void AnimFrameTool::Run()
{
  BOOL terminated = FALSE;
  while (!terminated)
  {
    ULONG sigs = 0;
    // Check for and handle any IntuiMessages
    if (sigs & (1 << m_pUserPort->mp_SigBit))
    {
      struct IntuiMessage *imsg;

      while (imsg = GT_GetIMsg(m_pUserPort))
      {
        terminated |= handleIntuiMessage(imsg);
        GT_ReplyIMsg(imsg);
      }
    }

    // Check for and handle any double-buffering messages. Note that
    // double-buffering messages are "replied" to us, so we don't want
    // to reply them to anyone.
    //
    if (sigs & (1 << m_pDBufPort->mp_SigBit))
    {
      struct Message *dbmsg;
      while (dbmsg = GetMsg(m_pDBufPort))
      {
        handleDBufMessage(dbmsg);
      }
    }


    if (!terminated)
    {
      ULONG held_off = 0;
      /* Only handle swapping buffers if count is non-zero */
      if (m_Count)
      {
        held_off = handleBufferSwap();
      }
      if (held_off)
      {
        /* If were held-off at ChangeScreenBuffer() time, then we
         * need to try ChangeScreenBuffer() again, without awaiting
         * a signal.  We WaitTOF() to avoid busy-looping.
         */
        WaitTOF();
      }
      else
      {
        /* If we were not held-off, then we're all done
         * with what we have to do.  We'll have no work to do
         * until some kind of signal arrives.  This will normally
         * be the arrival of the dbi_SafeMessage from the ROM
         * double-buffering routines, but it might also be an
         * IntuiMessage.
         */
        sigs = Wait((1 << m_pDBufPort->mp_SigBit) | (1 << m_pUserPort->mp_SigBit));
      }
    }
  }
}


/*----------------------------------------------------------------------*/

/* Handle the rendering and swapping of the buffers */

ULONG AnimFrameTool::handleBufferSwap()
{
  ULONG held_off = 0;
  /* 'buf_nextdraw' is the next buffer to draw into.
   * The buffer is ready for drawing when we've received the
   * dbi_SafeMessage for that buffer.  Our routine to handle
   * messaging from the double-buffering functions sets the
   * OK_REDRAW flag when this message has appeared.
   *
   * Here, we set the OK_SWAPIN flag after we've redrawn
   * the imagery, since the buffer is ready to be swapped in.
   * We clear the OK_REDRAW flag, since we're done with redrawing
   */
  if (m_Status[m_BufNextdraw] == OK_REDRAW)
  {
    x += xstep * xdir;
    if (x < 0)
    {
      x = 0;
      xdir = 1;
    }
    else if (x > m_pCanvasScreen->Width - BM_WIDTH)
    {
      x = m_pCanvasScreen->Width - BM_WIDTH - 1;
      xdir = -1;
    }

    y += ystep * ydir;
    if (y < m_pCanvasScreen->BarLayer->Height)
    {
      y = m_pCanvasScreen->BarLayer->Height;
      ydir = 1;
    }
    else if (y >= CONTROLSC_TOP - BM_HEIGHT)
    {
      y = CONTROLSC_TOP - BM_HEIGHT - 1;
      ydir = -1;
    }

    SetAPen(&m_RastPorts[m_BufNextdraw], 0);
    RectFill(&m_RastPorts[m_BufNextdraw],
      prevx[m_BufNextdraw], prevy[m_BufNextdraw],
      prevx[m_BufNextdraw] + BM_WIDTH - 1, prevy[m_BufNextdraw] + BM_HEIGHT - 1);
    prevx[m_BufNextdraw] = x;
    prevy[m_BufNextdraw] = y;

    BltBitMapRastPort(face, 0, 0, &m_RastPorts[m_BufNextdraw], x, y,
      BM_WIDTH, BM_HEIGHT, 0xc0);

    WaitBlit(); /* Gots to let the BBMRP finish */

    m_Status[m_BufNextdraw] = OK_SWAPIN;

    /* Toggle which the next buffer to draw is.
     * If you're using multiple ( >2 ) buffering, you
     * would use
     *
     *    buf_nextdraw = ( buf_nextdraw+1 ) % NUMBUFFERS;
     *
     */
    m_BufNextdraw ^= 1;
  }

  /* Let's make sure that the next frame is rendered before we swap...
   */
  if (m_Status[m_BufNextswap] == OK_SWAPIN)
  {
    m_pScreenBuffers[m_BufNextswap]->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = m_pDBufPort;

    if (ChangeScreenBuffer(m_pCanvasScreen, m_pScreenBuffers[m_BufNextswap]))
    {
      m_Status[m_BufNextswap] = 0;

      m_BufCurrent = m_BufNextswap;
      /* Toggle which the next buffer to swap in is.
       * If you're using multiple ( >2 ) buffering, you
       * would use
       *
       *    buf_nextswap = ( buf_nextswap+1 ) % NUMBUFFERS;
       *
       */
      m_BufNextswap ^= 1;

      m_Count--;
    }
    else
    {
      held_off = 1;
    }
  }
  return held_off;
}

/*----------------------------------------------------------------------*/



BOOL AnimFrameTool::handleIntuiMessage(struct IntuiMessage* pIntuiMsg)
{
  UWORD code = pIntuiMsg->Code;
  BOOL terminated = FALSE;

  switch (pIntuiMsg->Class)
  {
  case IDCMP_GADGETDOWN:
  case IDCMP_GADGETUP:
  case IDCMP_MOUSEMOVE:
    switch (((struct Gadget *)pIntuiMsg->IAddress)->GadgetID)
    {
    case GID_HSlide:
      xstep = code;
      break;

    case GID_VSlide:
      ystep = code;
      break;
    }
    break;

  case IDCMP_VANILLAKEY:
    switch (code)
    {
    case 'S':
    case 's':
      m_Count = 1;
      break;

    case 'R':
    case 'r':
      m_Count = ~0;
      break;

    case 'Q':
    case 'q':
      m_Count = 0;
      terminated = TRUE;
      break;
    }
    break;

  case IDCMP_MENUPICK:
    while (code != MENUNULL)
    {
      struct MenuItem *item;

      item = ItemAddress(m_pMenu, code);
      switch ((ULONG)GTMENUITEM_USERDATA(item))
      {
      case MID_Run:
        m_Count = ~0;
        break;

      case MID_Step:
        m_Count = 1;
        break;

      case MID_Quit:
        m_Count = 0;
        terminated = TRUE;
        break;

      case MID_HSlow:
        if (xstep > 0)
        {
          xstep--;
        }
        GT_SetGadgetAttrs(m_pGadgetSlideHorizontal, m_pControlWindow, NULL,
          GTSL_Level, xstep,
          TAG_DONE);
        break;

      case MID_HFast:
        if (xstep < 9)
        {
          xstep++;
        }
        GT_SetGadgetAttrs(m_pGadgetSlideHorizontal, m_pControlWindow, NULL,
          GTSL_Level, xstep,
          TAG_DONE);
        break;

      case MID_VSlow:
        if (ystep > 0)
        {
          ystep--;
        }
        GT_SetGadgetAttrs(m_pGadgetSlideVertical, m_pControlWindow, NULL,
          GTSL_Level, ystep,
          TAG_DONE);
        break;

      case MID_VFast:
        if (ystep < 9)
        {
          ystep++;
        }
        GT_SetGadgetAttrs(m_pGadgetSlideVertical, m_pControlWindow, NULL,
          GTSL_Level, ystep,
          TAG_DONE);
        break;
      }
      code = item->NextSelect;
    }
    break;
  }

  return terminated;
}


void AnimFrameTool::handleDBufMessage(struct Message* pDBufMsg)
{
  ULONG buffer;

  /* dbi_SafeMessage is followed by an APTR dbi_UserData1, which
   * contains the buffer number.  This is an easy way to extract
   * it.
   * The dbi_SafeMessage tells us that it's OK to redraw the
   * in the previous buffer.
   */
  buffer = (ULONG) *((APTR **)(pDBufMsg + 1));
  /* Mark the previous buffer as OK to redraw into.
   * If you're using multiple ( >2 ) buffering, you
   * would use
   *
   *    ( buffer + NUMBUFFERS - 1 ) % NUMBUFFERS
   *
   */
  m_Status[buffer ^ 1] = OK_REDRAW;
}


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/


#define MAXVECTORS  10

/**
 *  Draw a crude "face" for animation 
 */
struct BitMap* AnimFrameTool::makeImageBM()
{
  struct BitMap *bm;
  struct RastPort rport;
  struct AreaInfo area;
  struct TmpRas tmpRas;
  PLANEPTR planePtr;

  BYTE areabuffer[MAXVECTORS * 5];

  if (bm = (struct BitMap *)AllocBitMap(BM_WIDTH, BM_HEIGHT,
    BM_DEPTH, BMF_CLEAR, NULL))
  {
    if (planePtr = AllocRaster(BM_WIDTH, BM_HEIGHT))
    {
      InitRastPort(&rport);
      rport.BitMap = bm;

      InitArea(&area, areabuffer, MAXVECTORS);
      rport.AreaInfo = &area;

      InitTmpRas(&tmpRas, planePtr, RASSIZE(BM_WIDTH, BM_HEIGHT));
      rport.TmpRas = &tmpRas;

      SetABPenDrMd(&rport, 3, 0, JAM1);
      AreaEllipse(&rport, BM_WIDTH / 2, BM_HEIGHT / 2,
        BM_WIDTH / 2 - 4, BM_HEIGHT / 2 - 4);
      AreaEnd(&rport);

      SetAPen(&rport, 2);
      AreaEllipse(&rport, 5 * BM_WIDTH / 16, BM_HEIGHT / 4,
        BM_WIDTH / 9, BM_HEIGHT / 9);
      AreaEllipse(&rport, 11 * BM_WIDTH / 16, BM_HEIGHT / 4,
        BM_WIDTH / 9, BM_HEIGHT / 9);
      AreaEnd(&rport);

      SetAPen(&rport, 1);
      AreaEllipse(&rport, BM_WIDTH / 2, 3 * BM_HEIGHT / 4,
        BM_WIDTH / 3, BM_HEIGHT / 9);
      AreaEnd(&rport);

      FreeRaster(planePtr, BM_WIDTH, BM_HEIGHT);
    }
    else
    {
      FreeBitMap(bm);
      bm = NULL;
    }

    return bm;
  }
}


struct Gadget* AnimFrameTool::createAllGadgets(struct Gadget **ppGadgetList, 
                                               APTR pVisualInfo)
{
  struct NewGadget ng;
  struct Gadget *gad;

  gad = CreateContext(ppGadgetList);

  ng.ng_LeftEdge = 100;
  ng.ng_TopEdge = 1;
  ng.ng_Width = 100;
  ng.ng_Height = 12;
  ng.ng_GadgetText = (UBYTE*)"Horiz:  ";
  ng.ng_TextAttr = &Topaz80;
  ng.ng_VisualInfo = pVisualInfo;
  ng.ng_GadgetID = GID_HSlide;
  ng.ng_Flags = 0;

  m_pGadgetSlideHorizontal = gad = CreateGadget(SLIDER_KIND, gad, &ng,
    GTSL_Min, 0,
    GTSL_Max, 9,
    GTSL_Level, 1,
    GTSL_MaxLevelLen, 1,
    GTSL_LevelFormat, "%ld",
    TAG_DONE);

  ng.ng_LeftEdge += 200;
  ng.ng_GadgetID = GID_VSlide;
  ng.ng_GadgetText = (UBYTE*) "Vert:  ";
  m_pGadgetSlideVertical = gad = CreateGadget(SLIDER_KIND, gad, &ng,
    GTSL_Min, 0,
    GTSL_Max, 9,
    GTSL_Level, 1,
    GTSL_MaxLevelLen, 1,
    GTSL_LevelFormat, "%ld",
    TAG_DONE);

  return gad;
}


void AnimFrameTool::cleanup()
{
  if (m_pControlWindow != NULL)
  {
    ClearMenuStrip(m_pControlWindow);
    CloseWindowSafely(m_pControlWindow);
    m_pControlWindow = NULL;
  }

  if (m_pCanvasWindow != NULL)
  {
    ClearMenuStrip(m_pCanvasWindow);
    CloseWindowSafely(m_pCanvasWindow);
    m_pCanvasWindow = NULL;
  }

  if (m_pControlScreen != NULL)
  {
    CloseScreen(m_pControlScreen);
    m_pControlScreen = NULL;
  }

  if (m_pCanvasScreen != NULL)
  {
    FreeScreenBuffer(m_pCanvasScreen, m_pScreenBuffers[1]);
    FreeScreenBuffer(m_pCanvasScreen, m_pScreenBuffers[0]);
    CloseScreen(m_pCanvasScreen);
    m_pCanvasScreen = NULL;
  }

  if (m_pDBufPort != NULL)
  {
    DeleteMsgPort(m_pDBufPort);
    m_pDBufPort = NULL;
  }

  if (m_pUserPort != NULL)
  {
    DeleteMsgPort(m_pUserPort);
    m_pUserPort = NULL;
  }

  if (m_pGadgetList != NULL)
  {
    FreeGadgets(m_pGadgetList);
    m_pGadgetList = NULL;
  }

  if(m_pMenu != NULL)
  {
    FreeMenus(m_pMenu);
    m_pMenu = NULL;
  }

  if(m_pVisualInfoCanvas != NULL)
  {
    FreeVisualInfo(m_pVisualInfoCanvas);
    m_pVisualInfoCanvas = NULL;
  }

  if(m_pVisualInfoControl != NULL)
  {
    FreeVisualInfo(m_pVisualInfoControl);
    m_pVisualInfoControl = NULL;
  }

  if (face != NULL)
  {
    FreeBitMap(face);
    face = NULL;
  }
}


void AnimFrameTool::CloseWindowSafely(struct Window* pIntuiWindow)
{
  // we forbid here to keep out of race conditions with Intuition
  Forbid();

  // send back any messages for this window that have not yet been
  // processed
  StripIntuiMessages(pIntuiWindow->UserPort, pIntuiWindow);

  // clear UserPort so Intuition will not free it
  pIntuiWindow->UserPort = NULL;

  // tell Intuition to stop sending more messages
  ModifyIDCMP(pIntuiWindow, 0L);

  // turn multitasking back on
  Permit();

  // and really close the window
  CloseWindow(pIntuiWindow);
}


void AnimFrameTool::StripIntuiMessages(struct MsgPort* pMsgPort, 
                                       struct Window* pIntuiWindow)
{
  struct IntuiMessage* pMsg = (struct IntuiMessage *) pMsgPort->mp_MsgList.lh_Head;

  struct Node* pSuccessor = NULL;
  while ((pSuccessor = pMsg->ExecMessage.mn_Node.ln_Succ) != NULL)
  {
    if (pMsg->IDCMPWindow == pIntuiWindow)
    {
      // Intuition is about to free this message. Make sure that we have
      // politely sent it back.
      Remove((struct Node *)pMsg);
      ReplyMsg((struct Message *)pMsg);
    }

    pMsg = (struct IntuiMessage *) pSuccessor;
  }
}
