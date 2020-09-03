#include <exec/types.h>
#include <libraries/gadtools.h>
#include <graphics/videocontrol.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>


#include "AslFileRequest.h"
#include "MessageBox.h"

#include "AnimFrameTool.h"


#define VIEW_MODE_ID              LORES_KEY

#define CANVAS_HEIGHT             112

#define UI_RASTER_WIDTH           5
#define UI_RASTER_HEIGHT          20
#define UI_LABEL_WIDTH            96
#define UI_BEVBOX_BORDERS_WIDTH   4
#define UI_BEVBOX_WIDTH           96

#define OK_REDRAW 1  // Buffer fully detached, ready for redraw
#define OK_SWAPIN 2  // Buffer redrawn, ready for swap-in


/* Some constants to handle the rendering of the animated face */
#define BM_WIDTH  120
#define BM_HEIGHT 60
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
  : m_OScanWidth(0),
    m_OScanHeight(0),
    m_pLoadedPicture(NULL),
    m_pCanvasScreen(NULL),
    m_pControlScreen(NULL),
    m_pCanvasWindow(NULL),
    m_pControlWindow(NULL),
    m_pGadgetList(NULL),
    m_pGadgetSlideHScroll(NULL),
    m_pGadgetTextFilename(NULL),
    m_pGadgetFrameWidth(NULL),
    m_pGadgetButtonPlay(NULL),
    m_pGadgetButtonStop(NULL),
    m_pGadgetStringCurrFrame(NULL),
    m_pGadgetTextNumFrames(NULL),
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
  initialize();
}


AnimFrameTool::~AnimFrameTool()
{
  cleanup();
}

void AnimFrameTool::Run()
{
  ULONG sigs = 0;
  bool isTerminated = false;
  while (!isTerminated)
  {
    // Check for and handle any IntuiMessages
    if (sigs & (1 << m_pUserPort->mp_SigBit))
    {
      struct IntuiMessage* pIntuiMsg;

      while ((pIntuiMsg = GT_GetIMsg(m_pUserPort)) != NULL)
      {
        isTerminated |= handleIntuiMessage(pIntuiMsg);
        GT_ReplyIMsg(pIntuiMsg);
      }
    }

    // Check for and handle any double-buffering messages. Note that
    // double-buffering messages are "replied" to us, so we don't want
    // to reply them to anyone.
    if (sigs & (1 << m_pDBufPort->mp_SigBit))
    {
      struct Message* pDBufMsg;
      while ((pDBufMsg = GetMsg(m_pDBufPort)) != NULL)
      {
        handleDBufMessage(pDBufMsg);
      }
    }


    if (!isTerminated)
    {
      ULONG held_off = 0;
      // Only handle swapping buffers if count is non-zero
      if (m_Count)
      {
        held_off = handleBufferSwap();
      }
      if (held_off)
      {
        // If were held-off at ChangeScreenBuffer() time, then we
        // need to try ChangeScreenBuffer() again, without awaiting
        // a signal. We WaitTOF() to avoid busy-looping.
        // 
        WaitTOF();
      }
      else
      {
        // If we were not held-off, then we're all done with what we
        // have to do.  We'll have no work to do until some kind of
        // signal arrives.  This will normally be the arrival of the
        // dbi_SafeMessage from the ROM double-buffering routines, but
        // it might also be an IntuiMessage.
        sigs = Wait((1 << m_pDBufPort->mp_SigBit) | (1 << m_pUserPort->mp_SigBit));
      }
    }
  }
}



ULONG AnimFrameTool::handleBufferSwap()
{
  ULONG held_off = 0;

  // 'buf_nextdraw' is the next buffer to draw into. The buffer is ready
  // for drawing when we've received the dbi_SafeMessage for that
  // buffer. Our routine to handle messaging from the double-buffering
  // functions sets the OK_REDRAW flag when this message has appeared.
  //
  // Here, we set the OK_SWAPIN flag after we've redrawn the imagery,
  // since the buffer is ready to be swapped in. We clear the OK_REDRAW
  // flag, since we're done with redrawing
  
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
    else if (y >= CANVAS_HEIGHT - BM_HEIGHT)
    {
      y = CANVAS_HEIGHT - BM_HEIGHT - 1;
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

    // Toggle which the next buffer to draw is. If you're using multiple
    // ( >2 ) buffering, you would use
    //   buf_nextdraw = ( buf_nextdraw+1 ) % NUMBUFFERS;
    m_BufNextdraw ^= 1;
  }

  // Let's make sure that the next frame is rendered before we swap...
  if (m_Status[m_BufNextswap] == OK_SWAPIN)
  {
    m_pScreenBuffers[m_BufNextswap]->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = m_pDBufPort;

    if (ChangeScreenBuffer(m_pCanvasScreen, m_pScreenBuffers[m_BufNextswap]))
    {
      m_Status[m_BufNextswap] = 0;

      m_BufCurrent = m_BufNextswap;
      // Toggle which the next buffer to swap in is.
      // If you're using multiple ( >2 ) buffering, you
      // would use
      //  buf_nextswap = ( buf_nextswap+1 ) % NUMBUFFERS;
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


bool AnimFrameTool::handleIntuiMessage(struct IntuiMessage* pIntuiMsg)
{
  UWORD code = pIntuiMsg->Code;
  bool hasTerminated = false;

  switch (pIntuiMsg->Class)
  {
  case IDCMP_GADGETDOWN:
  case IDCMP_GADGETUP:
  case IDCMP_MOUSEMOVE:
    switch (((struct Gadget *)pIntuiMsg->IAddress)->GadgetID)
    {
    case GID_SlideHScroll:
      xstep = code;
      break;

    case GID_SlideFrameWordWidth:
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
      hasTerminated = true;
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
      case MID_ProjectOpenAnim:
        openAnim();
        break;

      case MID_ProjectSaveAnim:
        m_Count = ~0;  // Starts the anim face; TODO remove
        break;

      case MID_ProjectQuit:
        m_Count = 0;
        hasTerminated = TRUE;
        break;

      case MID_ProjectAbout:
        if (xstep > 0)
        {
          xstep--;
        }
        GT_SetGadgetAttrs(m_pGadgetSlideHScroll, m_pControlWindow, NULL,
          GTSL_Level, xstep,
          TAG_DONE);
        break;

      case MID_ToolsCenterAllFrames:
        if (xstep < 9)
        {
          xstep++;
        }
        GT_SetGadgetAttrs(m_pGadgetSlideHScroll, m_pControlWindow, NULL,
          GTSL_Level, xstep,
          TAG_DONE);
        break;

      case MID_ToolsGetMaxWidth:
        if (ystep < 9)
        {
          ystep++;
        }
        GT_SetGadgetAttrs(m_pGadgetFrameWidth, m_pControlWindow, NULL,
          GTSL_Level, ystep,
          TAG_DONE);
        break;
      }
      code = item->NextSelect;
    }
    break;
  }

  return hasTerminated;
}


void AnimFrameTool::handleDBufMessage(struct Message* pDBufMsg)
{
  ULONG buffer;

  // dbi_SafeMessage is followed by an APTR dbi_UserData1, which
  // contains the buffer number. This is an easy way to extract it. The
  // dbi_SafeMessage tells us that it's OK to redraw the in the previous
  // buffer.
  buffer = (ULONG) *((APTR **)(pDBufMsg + 1));
  // Mark the previous buffer as OK to redraw into.
  // If you're using multiple ( >2 ) buffering, you
  // would use
  //   ( buffer + NUMBUFFERS - 1 ) % NUMBUFFERS
  m_Status[buffer ^ 1] = OK_REDRAW;
}


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void AnimFrameTool::openAnim()
{
  AslFileRequest request(m_pControlWindow);
  std::string filename = request.SelectFile("Open anim picture", 
                                            "", 
                                            false);
  if(filename.length() > 0)
  {
    try
    {
      // Creating the IlbmBitmap will throw an exception on failure
      IlbmBitmap* pNewPicture = new IlbmBitmap(filename.c_str(), 
                                               true,
                                               false);

      if(m_pLoadedPicture != NULL)
      {
        delete m_pLoadedPicture;
      }

      m_pLoadedPicture = pNewPicture;

      m_Filename = filename;
      GT_SetGadgetAttrs(m_pGadgetTextFilename, m_pControlWindow, NULL,
                        GTTX_Text, m_Filename.c_str(),
                        TAG_DONE);

      cleanup();
      initialize();
    }
    catch(const char* pMsg)
    {
      std::string msgString = "The selected picture\n\n";
      msgString += filename;
      msgString += "\n\ncouldn't be loaded:\n\n";
      msgString += pMsg;

      MessageBox request;
      request.Show("Failed to load anim picture",
                   msgString.c_str(),
                   "Ok");
    }
    

  }
}


#define MAXVECTORS  100

/**
 *  Draw a crude "face" for animation 
 */
struct BitMap* AnimFrameTool::makeImageBM()
{
  struct BitMap *bm = NULL;
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

  return NULL;
}

LONG WordsToPixels(struct Gadget* pGadget, WORD level)
{
  return ((WORD)(level * 16));
}


void AnimFrameTool::initialize()
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
    {  NM_ITEM, "Open anim picture",  "O", 0, 0, (APTR)MID_ProjectOpenAnim, },
    {  NM_ITEM, "Open anim picture",  "S", 0, 0, (APTR)MID_ProjectSaveAnim, },
    {  NM_ITEM, NM_BARLABEL,           0 , 0, 0, 0, },
    {  NM_ITEM, "About",               0, 0, 0, (APTR)MID_ProjectAbout, },
    {  NM_ITEM, NM_BARLABEL,           0 , 0, 0, 0, },
    {  NM_ITEM, "Quit",               "Q", 0, 0, (APTR)MID_ProjectQuit, },
    { NM_TITLE, "Tools",               0 , 0, 0, 0, },
    {  NM_ITEM, "Center all frames",   0, 0, 0, (APTR)MID_ToolsCenterAllFrames, },
    {  NM_ITEM, "Get max width",       0, 0, 0, (APTR)MID_ToolsGetMaxWidth, },
    { NM_END,   0,                     0 , 0, 0, 0, },
  };

  const char* pScreenTitle = "Animation frame adjustment tool";

  if(m_pLoadedPicture != NULL)
  {
    ULONG screenWidth = m_pLoadedPicture->Width();
    if(screenWidth < m_OScanWidth)
    {
      screenWidth = m_OScanWidth;
    }

    m_pCanvasScreen = OpenScreenTags(NULL,
                                     SA_DisplayID, VIEW_MODE_ID,
                                     SA_Overscan, OSCAN_TEXT,
                                     SA_Width, screenWidth,
//                                     SA_Height, CANVAS_HEIGHT,
                                     SA_Depth, m_pLoadedPicture->Depth(),
                                     SA_Colors32, m_pLoadedPicture->GetColors32(),
                                     SA_AutoScroll, 1,
                                     SA_ShowTitle, TRUE,
                                     SA_Title, pScreenTitle,
                                     SA_VideoControl, vctags,
                                     SA_Font, &Topaz80,
                                     TAG_DONE);
  }
  else
  {
    m_pCanvasScreen = OpenScreenTags(NULL,
                                     SA_DisplayID, VIEW_MODE_ID,
                                     SA_Overscan, OSCAN_TEXT,
                                     SA_Depth, 2,
                                     SA_AutoScroll, 1,
                                     SA_Pens, pens,
                                     SA_ShowTitle, TRUE,
                                     SA_Title, pScreenTitle,
                                     SA_VideoControl, vctags,
                                     SA_Font, &Topaz80,
                                     TAG_DONE);
  }
  
  if(m_pCanvasScreen == NULL)
  {
    cleanup();
    throw "Couldn't open canvas screen.";
  }

  
  if(m_OScanHeight == 0)
  {
    // First screen opening, canvas screen is still empty:
    // Reading the overscan dimensions from its size
    m_OScanWidth = m_pCanvasScreen->Width;
    m_OScanHeight = m_pCanvasScreen->Height;

    // Calculate the two main rectangular areas of the control screen
    m_ResultFrameRect = Rect(m_OScanWidth - UI_RASTER_WIDTH - UI_BEVBOX_WIDTH - 4,
                            2 * UI_RASTER_HEIGHT);

    m_ResultFrameRect.SetWidthHeight(UI_BEVBOX_WIDTH, 
                                    UI_BEVBOX_WIDTH); // square -> height is width
    
    m_ControlsRect = Rect(UI_RASTER_WIDTH, 
                          m_ResultFrameRect.Top() - 1);
    
    m_ControlsRect.SetWidthHeight(m_ResultFrameRect.Left()
                                    - 2 // width of the bevel box border
                                    - UI_RASTER_WIDTH,
                                  m_ResultFrameRect.Bottom() 
                                    - m_ControlsRect.Top() 
                                    + 1);
  }

  if (!(m_pVisualInfoCanvas = GetVisualInfo(m_pCanvasScreen, TAG_DONE)))
  {
    cleanup();
    throw "Couldn't get VisualInfo of canvas screen.";
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
    throw "Couldn't open canvas window.";
  }

  m_pCanvasWindow->UserPort = m_pUserPort;

  ModifyIDCMP(m_pCanvasWindow, IDCMP_MENUPICK | IDCMP_VANILLAKEY);


  BltBitMapRastPort(m_pLoadedPicture->GetBitMap(), 
                    0, 0, 
                    m_pCanvasWindow->RPort, 
                    0, 0,
                    m_pLoadedPicture->Width(), CANVAS_HEIGHT, 
                    0xc0);

  WaitBlit();

  if (!(m_pControlScreen = OpenScreenTags(NULL,
    SA_DisplayID, VIEW_MODE_ID,
    SA_Overscan, OSCAN_TEXT,
    SA_Depth, 2,
    SA_Pens, pens,
    SA_Top, CANVAS_HEIGHT,
    SA_Height, m_OScanHeight-CANVAS_HEIGHT,
    SA_Parent, m_pCanvasScreen,
    SA_ShowTitle, FALSE,
    SA_Draggable, FALSE,
    SA_VideoControl, vctags,
    SA_Quiet, TRUE,
    SA_Font, &Topaz80,
    TAG_DONE)))
  {
    cleanup();
    throw "Couldn't open control screen.";
  }

  if (!(m_pVisualInfoControl = GetVisualInfo(m_pControlScreen, TAG_DONE)))
  {
    cleanup();
    throw "Couldn't get VisualInfo of control screen.";
  }

  if (!(m_pMenu = CreateMenus(demomenu,
    TAG_DONE)))
  {
    cleanup();
    throw "Couldn't create menus.";
  }

  if (!LayoutMenus(m_pMenu, m_pVisualInfoCanvas,
                   GTMN_NewLookMenus, TRUE,
                   TAG_DONE))
  {
    cleanup();
    throw "Couldn't layout menus.";
  }

  if (!createGadgets(&m_pGadgetList, m_pVisualInfoControl))
  {
    cleanup();
    throw "Couldn't create gadgets.";
  }

  // A borderless backdrop window so we can get input
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
    throw "Couldn't open control window.";
  }
  
  DrawBevelBox(m_pControlWindow->RPort, 
               m_ResultFrameRect.Left() - 2,
               m_ResultFrameRect.Top() - 1,
               m_ResultFrameRect.Width() + 4,
               m_ResultFrameRect.Height() + 2,
               GT_VisualInfo, m_pVisualInfoControl,
               GTBB_Recessed, TRUE,
               TAG_DONE);

  m_pControlWindow->UserPort = m_pUserPort;
  ModifyIDCMP(m_pControlWindow, SLIDERIDCMP | IDCMP_MENUPICK | IDCMP_VANILLAKEY);

  GT_RefreshWindow(m_pControlWindow, NULL);
  SetMenuStrip(m_pCanvasWindow, m_pMenu);
  LendMenus(m_pControlWindow, m_pCanvasWindow);

  if (!(m_pScreenBuffers[0] = AllocScreenBuffer(m_pCanvasScreen, NULL, SB_SCREEN_BITMAP)))
  {
    cleanup();
    throw "Couldn't allocate ScreenBuffer 1.";
  }

  if (!(m_pScreenBuffers[1] = AllocScreenBuffer(m_pCanvasScreen, NULL, SB_COPY_BITMAP)))
  {
    cleanup();
    throw "Couldn't allocate ScreenBuffer 2.";
  }

  // Let's use the UserData to store the buffer number, for easy
  // identification when the message comes back.
  m_pScreenBuffers[0]->sb_DBufInfo->dbi_UserData1 = (APTR)(0);
  m_pScreenBuffers[1]->sb_DBufInfo->dbi_UserData1 = (APTR)(1);
  m_Status[0] = OK_REDRAW;
  m_Status[1] = OK_REDRAW;

  if (!(face = makeImageBM()))
  {
    cleanup();
    throw "Couldn't allocate image bitmap.";
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

struct Gadget* AnimFrameTool::createGadgets(struct Gadget **ppGadgetList, 
                                            APTR pVisualInfo)
{
  struct NewGadget ng;
  struct Gadget* pGadget;

  const int rowHeight = 12;

  pGadget = CreateContext(ppGadgetList);

  ng.ng_LeftEdge = 0;
  ng.ng_TopEdge = 0;
  ng.ng_Width = m_OScanWidth;
  ng.ng_Height = rowHeight;
  ng.ng_TextAttr = &Topaz80;
  ng.ng_GadgetText = NULL;
  ng.ng_VisualInfo = pVisualInfo;
  ng.ng_GadgetID = GID_SlideHScroll;
  ng.ng_Flags = 0;

  m_pGadgetSlideHScroll = pGadget = CreateGadget(SLIDER_KIND, 
                                                 pGadget, 
                                                 &ng,
                                                 GTSL_Min, 0,
                                                 GTSL_Max, 0,
                                                 GTSL_Level, 0,
                                                 TAG_DONE);

  ng.ng_LeftEdge = m_ControlsRect.Left() + UI_LABEL_WIDTH;
  ng.ng_TopEdge += UI_RASTER_HEIGHT;
  ng.ng_Width = m_ResultFrameRect.Right() - ng.ng_LeftEdge + 2;

  ng.ng_GadgetID = GID_TextFilename;
  ng.ng_Flags = NG_HIGHLABEL;
  ng.ng_GadgetText = (UBYTE*) "File:    ";
  
  m_pGadgetTextFilename = pGadget = CreateGadget(TEXT_KIND, pGadget, &ng,
                                                 GTTX_Border, TRUE,
                                                 GTTX_Text, m_Filename.c_str(),
                                                 TAG_DONE);

  ng.ng_TopEdge = m_ControlsRect.Top();
  ng.ng_Width = m_ControlsRect.Width() - UI_LABEL_WIDTH - UI_RASTER_WIDTH;
  ng.ng_GadgetID = GID_SlideFrameWordWidth;
  ng.ng_GadgetText = (UBYTE*) "FWidth:    ";
  m_pGadgetFrameWidth = pGadget = CreateGadget(SLIDER_KIND, pGadget, &ng,
                                               GTSL_Min, 1,
                                               GTSL_Max, 16,
                                               GTSL_Level, 1,
                                               GTSL_MaxLevelLen, 3,
                                               GTSL_LevelFormat, "%ld",
                                               GTSL_MaxPixelLen, 24,
                                               GTSL_Justification, GTJ_RIGHT,
                                               GTSL_DispFunc, WordsToPixels,
                                               TAG_DONE);
  ng.ng_LeftEdge = m_ControlsRect.Left() + UI_LABEL_WIDTH;
  ng.ng_TopEdge += UI_RASTER_HEIGHT;
  ng.ng_Width = 36; // manually adjusted to fit this lines width
  ng.ng_GadgetID = GID_StringCurrentFrame;
  ng.ng_GadgetText = (UBYTE*) "Frame:    ";

  m_pGadgetStringCurrFrame = pGadget = CreateGadget(STRING_KIND,
                                                    pGadget,
                                                    &ng,
                                                    GTST_MaxChars, 2,
                                                    TAG_DONE);

  ng.ng_LeftEdge = m_ControlsRect.Right() - ng.ng_Width - UI_RASTER_WIDTH;

  ng.ng_GadgetID = GID_TextNumFrames;
  ng.ng_GadgetText = (UBYTE*) "of";

  m_pGadgetTextNumFrames = pGadget = CreateGadget(TEXT_KIND,
                                                  pGadget,
                                                  &ng,
                                                  GTTX_Border, TRUE,
                                                  TAG_DONE);  

  ng.ng_LeftEdge = m_ControlsRect.Left();
  ng.ng_TopEdge = m_ControlsRect.Bottom() - rowHeight;
  ng.ng_Width = (m_ControlsRect.Width() / 2) - UI_RASTER_WIDTH;
  ng.ng_GadgetID = GID_ButtonPlay;
  ng.ng_GadgetText = (UBYTE*) "Play";

  m_pGadgetButtonPlay = pGadget = CreateGadget(BUTTON_KIND, pGadget, &ng,
                                               TAG_DONE);

  ng.ng_LeftEdge += ng.ng_Width + UI_RASTER_WIDTH;
  ng.ng_GadgetID = GID_ButtonStop;
  ng.ng_GadgetText = (UBYTE*) "Stop";

  m_pGadgetButtonStop = pGadget = CreateGadget(BUTTON_KIND, pGadget, &ng,
                                               TAG_DONE);

  return pGadget;
}


void AnimFrameTool::cleanup()
{
  if(m_pLoadedPicture != NULL)
  {
    delete m_pLoadedPicture;
    m_pLoadedPicture = NULL;
  }

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
