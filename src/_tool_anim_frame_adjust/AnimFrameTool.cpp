#include <exec/types.h>
#include <libraries/gadtools.h>
#include <graphics/videocontrol.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>

#include <devices/input.h>

#include <stdio.h>


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


struct TextAttr Topaz80 =
{
  "topaz.font",               // Name
  8,                          // YSize
  FS_NORMAL,                  // Style
  FPF_ROMFONT | FPF_DESIGNED, // Flags
};


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

AnimFrameTool::AnimFrameTool()
  : m_OScanWidth(0),
    m_OScanHeight(0),
    m_pLoadedPicture(NULL),
    m_pBitMapTools(NULL),
    m_pCanvasScreen(NULL),
    m_pControlScreen(NULL),
    m_pCanvasWindow(NULL),
    m_pControlWindow(NULL),
    m_pGadgetList(NULL),
    m_pGadScrCanvasHScroll(NULL),
    m_pGadTxtFilename(NULL),
    m_pGadSliFrameWidth(NULL),
    m_pGadBtnPlay(NULL),
    m_pGadBtnStop(NULL),
    m_pGadIntCurrentFrame(NULL),
    m_pGadTxtNumFrames(NULL),
    m_pMenu(NULL),
    m_pVisualInfoCanvas(NULL),
    m_pVisualInfoControl(NULL),
    m_pUserPort(NULL)          
{
  if (!(m_pUserPort = CreateMsgPort()))
  {
    cleanup();
    throw "Failed to create port";
  }

  struct NewMenu demomenu[] =
  {
    { NM_TITLE, "Project",             0 , 0, 0, 0, },
    {  NM_ITEM, "Open anim picture",  "O", 0, 0, (APTR)MID_ProjectOpenAnim, },
    {  NM_ITEM, "Save anim picture",  "S", 0, 0, (APTR)MID_ProjectSaveAnim, },
    {  NM_ITEM, NM_BARLABEL,           0 , 0, 0, 0, },
    {  NM_ITEM, "About",               0, 0, 0, (APTR)MID_ProjectAbout, },
    {  NM_ITEM, NM_BARLABEL,           0 , 0, 0, 0, },
    {  NM_ITEM, "Quit",               "Q", 0, 0, (APTR)MID_ProjectQuit, },
    { NM_TITLE, "Tools",               0 , 0, 0, 0, },
    {  NM_ITEM, "Center all frames",   0, 0, 0, (APTR)MID_ToolsCenterAllFrames, },
    {  NM_ITEM, "Get max width",       0, 0, 0, (APTR)MID_ToolsGetMaxWidth, },
    { NM_END,   0,                     0 , 0, 0, 0, },
  };

  if (!(m_pControlScreen = OpenScreenTags(NULL,
    SA_DisplayID, VIEW_MODE_ID,
    SA_Overscan, OSCAN_TEXT,
    SA_Depth, 2,
    SA_Pens, pens,
    SA_Title, "Animation frame adjustment tool",
    SA_VideoControl, vctags,
    SA_Font, &Topaz80,
    TAG_DONE)))
  {
    cleanup();
    throw "Couldn't open control screen.";
  }

  // Until the canvas is opened the control screen fills the whole
  // display. Read the overscan dimensions from its size.
  m_OScanWidth = m_pControlScreen->Width;
  m_OScanHeight = m_pControlScreen->Height;

  // Calculate the two main rectangular areas of the control screen
  m_ResultFrameRect = Rect(m_OScanWidth - UI_RASTER_WIDTH - UI_BEVBOX_WIDTH - 4,
                           m_pControlScreen->BarHeight + UI_RASTER_HEIGHT + 6); // TODO maybe only UI_RASTER_HEIGHT / 2 ??

  m_ResultFrameRect.SetWidthHeight(UI_BEVBOX_WIDTH, 
                                   UI_BEVBOX_WIDTH); // square -> height is width
  
  m_ControlsRect = Rect(UI_RASTER_WIDTH, 
                        m_ResultFrameRect.Top() - 1);
  
  m_ControlsRect.SetWidthHeight(m_ResultFrameRect.Left()
                                  - 2 // width of the bevel box border
                                  - UI_RASTER_WIDTH,
                                m_ResultFrameRect.Bottom() 
                                  - m_ControlsRect.Top() 
                                  + 4);

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

  if (!LayoutMenus(m_pMenu, m_pVisualInfoControl,
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
               m_ResultFrameRect.Width() + 5,
               m_ResultFrameRect.Height() + 3,
               GT_VisualInfo, m_pVisualInfoControl,
               GTBB_Recessed, TRUE,
               TAG_DONE);

  m_pControlWindow->UserPort = m_pUserPort;
  ModifyIDCMP(m_pControlWindow, SLIDERIDCMP 
                                | IDCMP_MENUPICK 
                                | IDCMP_VANILLAKEY
                                | IDCMP_RAWKEY);

  GT_RefreshWindow(m_pControlWindow, NULL);
  SetMenuStrip(m_pControlWindow, m_pMenu);

  openCanvas();

  paintGrid();
}


AnimFrameTool::~AnimFrameTool()
{
  cleanup();
}


void AnimFrameTool::Run()
{
  ULONG sigIDCMP = (1ul << m_pUserPort->mp_SigBit);
  ULONG signals = sigIDCMP; 
  bool hasTerminated = false;

  do
  {
    const ULONG received = Wait(signals);
    if(received & sigIDCMP)
    {
      struct IntuiMessage* pIntuiMsg;
      while ((pIntuiMsg = GT_GetIMsg(m_pUserPort)) != NULL)
      {
        hasTerminated |= handleIntuiMessage(pIntuiMsg);
        GT_ReplyIMsg(pIntuiMsg);
      }
    }
  }
  while(!hasTerminated);
}



void AnimFrameTool::moveFrameContentLeft()
{
  if(m_pLoadedPicture == NULL)
  {
    return;
  }

  // Move object (non-zero pixels) of current frame left by 1 pixel
  if(m_pBitMapTools->MoveObjectLeft(m_FrameRects[m_FrameId], 1) < 1)
  {
    return;
  }

  paintPicture();
  paintGrid();
  paintCurrentFrameToResultRect();
}


void AnimFrameTool::moveFrameContentRight()
{
  if(m_pLoadedPicture == NULL || m_pBitMapTools == NULL)
  {
    return;
  }

  // Move object (non-zero pixels) of current frame left by 1 pixel
  if(m_pBitMapTools->MoveObjectRight(m_FrameRects[m_FrameId], 1) < 1)
  {
    return;
  }

  paintPicture();
  paintGrid();
  paintCurrentFrameToResultRect();
}


void AnimFrameTool::moveFrameContentUp()
{
  if(m_pLoadedPicture == NULL)
  {
    return;
  }

  // Move object (non-zero pixels) of current frame left by 1 pixel
  if(m_pBitMapTools->MoveObjectUp(m_FrameRects[m_FrameId], 1) < 1)
  {
    return;
  }

  paintPicture();
  paintGrid();
  paintCurrentFrameToResultRect();
}


void AnimFrameTool::moveFrameContentDown()
{
  if(m_pLoadedPicture == NULL)
  {
    return;
  }

  // Move object (non-zero pixels) of current frame left by 1 pixel
  if(m_pBitMapTools->MoveObjectDown(m_FrameRects[m_FrameId], 1) < 1)
  {
    return;
  }

  paintPicture();
  paintGrid();
  paintCurrentFrameToResultRect();
}


void AnimFrameTool::gadgetFrameWidthChanged()
{
  calcFrameRects();
  paintPicture();
  paintGrid();

  // Clear result rect
  SetAPen(m_pControlWindow->RPort, 0);
  RectFill(m_pControlWindow->RPort, 
          m_ResultFrameRect.Left(),
          m_ResultFrameRect.Top(),
          m_ResultFrameRect.Right(),
          m_ResultFrameRect.Bottom());

  paintCurrentFrameToResultRect();
}


void AnimFrameTool::gadgetCurrentFrameChanged()
{
  ULONG newFrameNum;
  if(1 != GT_GetGadgetAttrs(m_pGadIntCurrentFrame, 
                            m_pControlWindow, 
                            NULL,
                            GTIN_Number, &newFrameNum,
                            TAG_DONE))
  {
    return;
  }

  if((newFrameNum < 1) || (newFrameNum > m_FrameRects.size()))
  {
    // User input frame number is too small or too big
    updateFrameIdxGadgets(true);
    return;
  }

  int newFrameId = newFrameNum - 1;

  paintSelectionRect(m_FrameRects[m_FrameId], false);
  paintSelectionRect(m_FrameRects[newFrameId], true);
  m_FrameId = newFrameId;

  paintCurrentFrameToResultRect();
}


void AnimFrameTool::selectPreviousFrame()
{
  int prevIndex = m_FrameId - 1;
  if(prevIndex < 0)
  {
    prevIndex = m_FrameRects.size() - 1;
  }

  paintSelectionRect(m_FrameRects[m_FrameId], false);
  paintSelectionRect(m_FrameRects[prevIndex], true);
  m_FrameId = prevIndex;

  updateFrameIdxGadgets(true);
  
  paintCurrentFrameToResultRect();
}


void AnimFrameTool::selectNextFrame()
{
  int nextIndex = m_FrameId + 1;
  if(nextIndex >= (int)m_FrameRects.size())
  {
    nextIndex = 0;
  }

  paintSelectionRect(m_FrameRects[m_FrameId], false);
  paintSelectionRect(m_FrameRects[nextIndex], true);
  m_FrameId = nextIndex;

  updateFrameIdxGadgets(true);

  paintCurrentFrameToResultRect();
}


void AnimFrameTool::openAnimIlbmPicture()
{
  AslFileRequest request(m_pControlWindow);
  std::string filename = request.SelectFile("Open anim picture", 
                                            "", 
                                            false);
  std::string msgString;
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

      if(m_pBitMapTools != NULL)
      {
        delete m_pBitMapTools;
      }

      m_pLoadedPicture = pNewPicture;

      // Now create a BitMapTools instance to allow some
      // operations to the picture. FIXME: Remove the ugly cast.
      m_pBitMapTools = new BitMapTools((BitMap*)m_pLoadedPicture->GetBitMap());

      m_Filename = filename;
      GT_SetGadgetAttrs(m_pGadTxtFilename, m_pControlWindow, NULL,
                        GTTX_Text, m_Filename.c_str(),
                        TAG_DONE);


      closeCanvas();
      openCanvas();

      paintPicture();

      calcFrameRects();
      paintGrid();

      // Clear result rect
      SetAPen(m_pControlWindow->RPort, 0);
      RectFill(m_pControlWindow->RPort, 
               m_ResultFrameRect.Left(),
               m_ResultFrameRect.Top(),
               m_ResultFrameRect.Right(),
               m_ResultFrameRect.Bottom());

      paintCurrentFrameToResultRect();

    }
    catch(const char* pMsg)
    {
      msgString = "Failed to load \n  '";
      msgString += filename;
      msgString += "'\nas ilbm picture.\n\n";
      msgString += pMsg;

      MessageBox request(m_pControlWindow);
      request.Show("Loading error",
                   msgString.c_str(),
                   "Ok");
    }
  }
}

void AnimFrameTool::calcFrameRects()
{
  if(m_pLoadedPicture == NULL)
  {
    return;
  }

  m_FrameId = 0;

  LONG wordWidth;
  if(1 != GT_GetGadgetAttrs(m_pGadSliFrameWidth, 
                            m_pControlWindow, 
                            NULL,
                            GTSL_Level, &wordWidth,
                            TAG_DONE))
  {
    return;
  }

  int frameWidth = wordWidth * 16;
  if(frameWidth == 0)
  {
    return;
  }

  m_FrameRects.clear();
  
  // Create the needed number of yet uninitialized Rects
  m_NumFrames = m_pLoadedPicture->Width() / frameWidth;
  for(int i = 0; i < m_NumFrames; i++)
  {
    m_FrameRects.push_back(Rect());
  }

  // Initialize the rects
  for(int i = 0; i < m_NumFrames; i++)
  {
    m_FrameRects[i].Set(i * frameWidth,                   // left
                        0,                                // top
                        ((i+1) * frameWidth) - 1,         // right
                        m_pLoadedPicture->Height() - 1);  // bottom
  }

  updateFrameIdxGadgets(false);
}

void AnimFrameTool::updateFrameIdxGadgets(bool bCurrentOnly)
{

  GT_SetGadgetAttrs(m_pGadIntCurrentFrame, m_pControlWindow, NULL,
                    GTIN_Number, m_FrameId + 1,
                    TAG_DONE);

  if(bCurrentOnly)
  {
    return;
  }

  char buf[16];
  sprintf(buf, "%d", m_NumFrames);
  GT_SetGadgetAttrs(m_pGadTxtNumFrames, m_pControlWindow, NULL,
                    GTTX_Text, buf,
                    GTTX_CopyText, TRUE,
                    TAG_DONE);
}

void AnimFrameTool::paintPicture()
{
  if(m_pLoadedPicture == NULL || m_pCanvasWindow == NULL)
  {
    return;
  }

  SetRast(m_pCanvasWindow->RPort, 0);

  BltBitMapRastPort(m_pLoadedPicture->GetBitMap(), 
                    0, 0, 
                    &m_pCanvasScreen->RastPort, 
                    0, 0,
                    m_pLoadedPicture->Width(), m_pLoadedPicture->Height(), 
                    0xc0);

  WaitBlit();
}


void AnimFrameTool::paintGrid()
{
  if(m_pLoadedPicture == NULL || m_pCanvasWindow == NULL)
  {
    return;
  }

  // Set the pens for the grid
  m_NormalRectPen = 1;
  m_HighlightedRectPen = (1L << m_pLoadedPicture->Depth()) - 1;

  // Draw all m_FrameRect's
  for(size_t i = 0; i < m_FrameRects.size(); i++)
  {
    // For current frame set a different pen color (the highest pen available)
    bool isHighlighted = ((int)i == m_FrameId ? true : false);
    paintSelectionRect(m_FrameRects[i], isHighlighted);
  }
}

void AnimFrameTool::paintSelectionRect(const Rect& rect, 
                                      bool isHighlighted)
{
  if(isHighlighted)
  {
    SetAPen(m_pCanvasWindow->RPort, m_HighlightedRectPen);
  }
  else
  {
    SetAPen(m_pCanvasWindow->RPort, m_NormalRectPen);
  }
  
  Move(m_pCanvasWindow->RPort, rect.Left(), rect.Top());
  Draw(m_pCanvasWindow->RPort, rect.Left(), rect.Bottom());
  Draw(m_pCanvasWindow->RPort, rect.Right(), rect.Bottom());
  Draw(m_pCanvasWindow->RPort, rect.Right(), rect.Top());
  Draw(m_pCanvasWindow->RPort, rect.Left(), rect.Top());
}


void AnimFrameTool::paintCurrentFrameToResultRect()
{
  if((m_pLoadedPicture == NULL) || (m_pControlWindow == NULL))
  {
    return;
  }

  BltBitMapRastPort(m_pLoadedPicture->GetBitMap(), 
                    m_FrameRects[m_FrameId].Left(),
                    m_FrameRects[m_FrameId].Top(), 
                    m_pControlWindow->RPort, 
                    m_ResultFrameRect.Left(), m_ResultFrameRect.Top(),
                    m_FrameRects[m_FrameId].Width(), 
                    m_FrameRects[m_FrameId].Height(), 
                    0xc0);
}


bool AnimFrameTool::handleIntuiMessage(struct IntuiMessage* pIntuiMsg)
{
  UWORD code = pIntuiMsg->Code;
  ULONG shiftQualifier = IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT;
  bool hasTerminated = false;

  switch (pIntuiMsg->Class)
  {
  case IDCMP_GADGETDOWN:
  case IDCMP_GADGETUP:
  case IDCMP_MOUSEMOVE:
    switch (((struct Gadget *)pIntuiMsg->IAddress)->GadgetID)
    {
    case GID_ScrCanvasHoriz:
    {
      // Get new top position of the scroller
      ULONG newTop;
      GT_GetGadgetAttrs(m_pGadScrCanvasHScroll, m_pControlWindow, NULL, 
                        GTSC_Top, &newTop,
                        TAG_DONE);
      
      // Scroll canvas screens viewport to the new position
      m_pCanvasScreen->ViewPort.RasInfo->RxOffset = newTop;
      ScrollVPort(&m_pCanvasScreen->ViewPort);
      break;

    }

    case GID_SliFrameWordWidth:
      {
        gadgetFrameWidthChanged();
        break;
      }
    
    case GID_IntCurrentFrame:
      {
        gadgetCurrentFrameChanged();
        break;
      }
    }
    break;

  case IDCMP_VANILLAKEY:
    switch (code)
    {
    case 'S':
    case 's':
      // TODO
      break;

    case 'R':
    case 'r':
      // TODO
      break;

    case 'Q':
    case 'q':
      // TODO
      hasTerminated = true;
      break;
    }
    break;

  case IDCMP_RAWKEY:
    switch (code)
    {
    case 0x4f:
      // Cursor left
      if(pIntuiMsg->Qualifier & shiftQualifier)
      {
        moveFrameContentLeft();
      }
      else
      {
        selectPreviousFrame();
      }
      break;

    case 0x4e:
      // Cursor right
      if(pIntuiMsg->Qualifier & shiftQualifier)
      {
        moveFrameContentRight();
      }
      else
      {
        selectNextFrame();
      }
      break;

    case 0x4c:
      // Cursor Up
      if(pIntuiMsg->Qualifier & shiftQualifier)
      {
        moveFrameContentUp();
      }
      break;

    case 0x4d:
      // Cursor Down

      if(pIntuiMsg->Qualifier & shiftQualifier)
      {
        moveFrameContentDown();
      }
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
        openAnimIlbmPicture();
        break;

      case MID_ProjectSaveAnim:
        // TODO
        break;

      case MID_ProjectQuit:
        hasTerminated = TRUE;
        break;

      case MID_ProjectAbout:
        // TODO
        break;

      case MID_ToolsCenterAllFrames:
        // TODO
        break;

      case MID_ToolsGetMaxWidth:
        // TODO
        break;
      }
      code = item->NextSelect;
    }
    break;
  }

  return hasTerminated;
}


void AnimFrameTool::cleanup()
{
  if(m_pBitMapTools != NULL)
  {
    delete m_pBitMapTools;
    m_pBitMapTools = NULL;
  }

  if(m_pLoadedPicture != NULL)
  {
    delete m_pLoadedPicture;
    m_pLoadedPicture = NULL;
  }

  if (m_pControlWindow != NULL)
  {
    ClearMenuStrip(m_pControlWindow);
    closeWindowSafely(m_pControlWindow);
    m_pControlWindow = NULL;
  }

  closeCanvas();

  if (m_pControlScreen != NULL)
  {
    CloseScreen(m_pControlScreen);
    m_pControlScreen = NULL;
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
}


void AnimFrameTool::openCanvas()
{
  if(m_pControlWindow == NULL)
  {
    return;
  }

  if(m_pLoadedPicture != NULL)
  {
    ULONG screenWidth = m_pLoadedPicture->Width();
    if(screenWidth < m_OScanWidth)
    {
      screenWidth = m_OScanWidth;
    }

    m_pCanvasScreen = OpenScreenTags(NULL,
                                     SA_AutoScroll, 1,
                                     SA_Colors32, m_pLoadedPicture->GetColors32(),
                                     SA_DisplayID, VIEW_MODE_ID,
                                     SA_Depth, m_pLoadedPicture->Depth(),
                                     SA_Draggable, FALSE,
                                     SA_Interleaved, TRUE,
                                     SA_Font, &Topaz80,
                                     SA_Parent, m_pControlScreen,
                                     SA_Quiet, TRUE,
                                     SA_ShowTitle, FALSE,
                                     SA_Top, m_pControlScreen->Height - CANVAS_HEIGHT,
                                     SA_VideoControl, vctags,
                                     SA_Width, screenWidth,
                                     TAG_DONE);
  }
  else
  {
    m_pCanvasScreen = OpenScreenTags(NULL,
                                     SA_AutoScroll, 1,
                                     SA_Depth, 2,
                                     SA_DisplayID, VIEW_MODE_ID,
                                     SA_Draggable, FALSE,
                                     SA_Interleaved, TRUE,
                                     SA_Font, &Topaz80,
                                     SA_Overscan, OSCAN_TEXT,
                                     SA_Parent, m_pControlScreen,
                                     SA_Pens, pens,
                                     SA_Quiet, TRUE,
                                     SA_ShowTitle, FALSE,
                                     SA_Top, m_pControlScreen->Height - CANVAS_HEIGHT,
                                     SA_VideoControl, vctags,
                                     TAG_DONE);
  }
  
  if(m_pCanvasScreen == NULL)
  {
    cleanup();
    throw "Couldn't open canvas screen.";
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
  ModifyIDCMP(m_pCanvasWindow, IDCMP_MENUPICK | IDCMP_VANILLAKEY | IDCMP_RAWKEY);
  LendMenus(m_pCanvasWindow, m_pControlWindow);

  // Adjust the scroller gadget to left<->right scroll the screen
  if(m_pLoadedPicture != NULL)
  {
    GT_SetGadgetAttrs(m_pGadScrCanvasHScroll, m_pControlWindow, NULL,
                      GTSC_Top, 0,
                      GTSC_Total, m_pLoadedPicture->Width(),
                      GTSC_Visible, m_pControlScreen->Width,
                      TAG_DONE);
  }
}


void AnimFrameTool::closeCanvas()
{
  if (m_pCanvasWindow != NULL)
  {
    ClearMenuStrip(m_pCanvasWindow);
    closeWindowSafely(m_pCanvasWindow);
    m_pCanvasWindow = NULL;
  }

  if (m_pCanvasScreen != NULL)
  {
    CloseScreen(m_pCanvasScreen);
    m_pCanvasScreen = NULL;
  }
}


LONG wordsToPixels(struct Gadget* pGadget, WORD level)
{
  return ((WORD)(level * 16));
}


struct Gadget* AnimFrameTool::createGadgets(struct Gadget **ppGadgetList, 
                                            APTR pVisualInfo)
{
  struct NewGadget ng;
  struct Gadget* pGadget;

  const int rowHeight = 12;

  pGadget = CreateContext(ppGadgetList);

  ng.ng_LeftEdge = m_ControlsRect.Left() + UI_LABEL_WIDTH;
  ng.ng_TopEdge = m_ControlsRect.Top() - UI_RASTER_HEIGHT;
  ng.ng_Width = m_ResultFrameRect.Right() - ng.ng_LeftEdge + 3;
  ng.ng_Height = rowHeight;
  ng.ng_TextAttr = &Topaz80;
  ng.ng_VisualInfo = pVisualInfo;
  ng.ng_GadgetID = GID_TxtFilename;
  ng.ng_Flags = NG_HIGHLABEL;
  ng.ng_GadgetText = (UBYTE*) "File:    ";
  
  m_pGadTxtFilename = pGadget = CreateGadget(TEXT_KIND, pGadget, &ng,
                                             GTTX_Border, TRUE,
                                             GTTX_Text, m_Filename.c_str(),
                                             TAG_DONE);

  ng.ng_TopEdge += UI_RASTER_HEIGHT;
  ng.ng_Width = m_ControlsRect.Width() - UI_LABEL_WIDTH - UI_RASTER_WIDTH;
  ng.ng_GadgetID = GID_SliFrameWordWidth;
  ng.ng_GadgetText = (UBYTE*) "FWidth:    ";
  m_pGadSliFrameWidth = pGadget = CreateGadget(SLIDER_KIND, pGadget, &ng,
                                               GTSL_Min, 1,
                                               GTSL_Max, 16,
                                               GTSL_Level, 1,
                                               GTSL_MaxLevelLen, 3,
                                               GTSL_LevelFormat, "%ld",
                                               GTSL_MaxPixelLen, 24,
                                               GTSL_Justification, GTJ_RIGHT,
                                               GTSL_DispFunc, wordsToPixels,
                                               TAG_DONE);
  ng.ng_LeftEdge = m_ControlsRect.Left() + UI_LABEL_WIDTH;
  ng.ng_TopEdge += UI_RASTER_HEIGHT;
  ng.ng_Width = 36; // manually adjusted to fit this lines width
  ng.ng_GadgetID = GID_IntCurrentFrame;
  ng.ng_GadgetText = (UBYTE*) "Frame:    ";

  m_pGadIntCurrentFrame = pGadget = CreateGadget(INTEGER_KIND,
                                                 pGadget,
                                                 &ng,
                                                 GTST_MaxChars, 2,
                                                 TAG_DONE);

  ng.ng_LeftEdge = m_ControlsRect.Right() - ng.ng_Width - UI_RASTER_WIDTH;

  ng.ng_GadgetID = GID_TxtNumFrames;
  ng.ng_GadgetText = (UBYTE*) "of";

  m_pGadTxtNumFrames = pGadget = CreateGadget(TEXT_KIND,
                                              pGadget,
                                              &ng,
                                              GTTX_Border, TRUE,
                                              TAG_DONE);  

  ng.ng_LeftEdge = m_ControlsRect.Left();
  ng.ng_TopEdge = m_ControlsRect.Bottom() - rowHeight;
  ng.ng_Width = (m_ControlsRect.Width() / 2) - UI_RASTER_WIDTH;
  ng.ng_GadgetID = GID_BtnPlay;
  ng.ng_GadgetText = (UBYTE*) "Play";

  m_pGadBtnPlay = pGadget = CreateGadget(BUTTON_KIND, pGadget, &ng,
                                         TAG_DONE);

  ng.ng_LeftEdge += ng.ng_Width + UI_RASTER_WIDTH;
  ng.ng_GadgetID = GID_BtnStop;
  ng.ng_GadgetText = (UBYTE*) "Stop";

  m_pGadBtnStop = pGadget = CreateGadget(BUTTON_KIND, pGadget, &ng,
                                         TAG_DONE);

  ng.ng_LeftEdge = 0;
  ng.ng_TopEdge = m_pControlScreen->Height - CANVAS_HEIGHT - rowHeight - 3; // Bottom of control screen
  ng.ng_Width = m_OScanWidth;
  ng.ng_GadgetText = NULL;
  ng.ng_GadgetID = GID_ScrCanvasHoriz;
  ng.ng_Flags = 0;

  m_pGadScrCanvasHScroll = pGadget = CreateGadget(SCROLLER_KIND, 
                                                  pGadget, 
                                                  &ng,
                                                  GTSC_Top, 0,
                                                  GTSC_Total, 0,
                                                  GTSC_Visible, 0,
                                                  GTSC_Arrows, rowHeight + 1,
                                                  TAG_DONE);

  return pGadget;
}




void AnimFrameTool::closeWindowSafely(struct Window* pIntuiWindow)
{
  // we forbid here to keep out of race conditions with Intuition
  Forbid();

  // send back any messages for this window that have not yet been
  // processed
  stripIntuiMessages(pIntuiWindow->UserPort, pIntuiWindow);

  // clear UserPort so Intuition will not free it
  pIntuiWindow->UserPort = NULL;

  // tell Intuition to stop sending more messages
  ModifyIDCMP(pIntuiWindow, 0L);

  // turn multitasking back on
  Permit();

  // and really close the window
  CloseWindow(pIntuiWindow);
}


void AnimFrameTool::stripIntuiMessages(struct MsgPort* pMsgPort, 
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