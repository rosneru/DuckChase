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
#include "ShadowMask.h"

#include "AnimFrameTool.h"

#define VIEW_MODE_ID              LORES_KEY

#define CANVAS_HEIGHT             112

#define UI_RASTER_WIDTH           5
#define UI_RASTER_HEIGHT          20
#define UI_LABEL_WIDTH            96
#define UI_BEVBOX_BORDERS_WIDTH   4
#define UI_BEVBOX_WIDTH           89

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
    m_pAnimSheets(NULL),
    m_pBitMapTools(NULL),
    m_HasChanged(false),
    m_SheedId(0),
    m_SheetNumFrames(0),
    m_FrameId(0),
    m_pCanvasScreen(NULL),
    m_pControlScreen(NULL),
    m_pCanvasWindow(NULL),
    m_pControlWindow(NULL),
    m_pGadgetList(NULL),
    m_pGadScrCanvasHScroll(NULL),
    m_pGadTxtFilename(NULL),
    m_pGadSliFrameWidth(NULL),
    m_pGadLvSheet(NULL),
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
    { NM_TITLE, "Project",                       0 , 0, 0, 0, },
    {  NM_ITEM, "Open anim picture",            "O", 0, 0, (APTR)MID_ProjectOpenAnim, },
    {  NM_ITEM, "Save",                         "S", 0, 0, (APTR)MID_ProjectSave, },
    {  NM_ITEM, NM_BARLABEL,                     0 , 0, 0, 0, },
    {  NM_ITEM, "About",                         0,  0, 0, (APTR)MID_ProjectAbout, },
    {  NM_ITEM, NM_BARLABEL,                     0 , 0, 0, 0, },
    {  NM_ITEM, "Quit",                         "Q", 0, 0, (APTR)MID_ProjectQuit, },
    { NM_TITLE, "Tools",                         0 , 0, 0, 0, },
    {  NM_ITEM, "Center all frames",             0,  0, 0, (APTR)MID_ToolsCenterAllFrames, },
    {  NM_ITEM, "Get max width",                 0,  0, 0, (APTR)MID_ToolsGetMaxWidth, },
    {  NM_ITEM, NM_BARLABEL,                     0 , 0, 0, 0, },
    {  NM_ITEM, "Print frame mask",             "F", 0, 0, (APTR)MID_ToolsPrintFrameMask, },
    {  NM_ITEM, "Print mask of whole picture",  "M", 0, 0, (APTR)MID_ToolsPrintFullMask, },
    { NM_END,   0,                               0 , 0, 0, 0, },
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

  disableMenuItem(MID_ProjectSave);

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


void AnimFrameTool::markChanged()
{
  if(m_HasChanged == true)
  {
    return;
  }

  m_HasChanged = true;
  enableMenuItem(MID_ProjectSave);
}


void AnimFrameTool::moveFrameContentLeft()
{
  if(m_pAnimSheets == NULL)
  {
    return;
  }

  // Move object (non-zero pixels) of current frame left by 1 pixel
  if(m_pBitMapTools->MoveObjectLeft(m_FrameRects[m_FrameId], 1) < 1)
  {
    return;
  }

  markChanged();

  paintPictureCurrentPart();
  paintCurrentFrameToResultRect();
}


void AnimFrameTool::moveFrameContentRight()
{
  if(m_pAnimSheets == NULL || m_pBitMapTools == NULL)
  {
    return;
  }

  // Move object (non-zero pixels) of current frame left by 1 pixel
  if(m_pBitMapTools->MoveObjectRight(m_FrameRects[m_FrameId], 1) < 1)
  {
    return;
  }

  markChanged();

  paintPictureCurrentPart();
  paintCurrentFrameToResultRect();
}


void AnimFrameTool::moveFrameContentUp()
{
  if(m_pAnimSheets == NULL)
  {
    return;
  }

  // Move object (non-zero pixels) of current frame left by 1 pixel
  if(m_pBitMapTools->MoveObjectUp(m_FrameRects[m_FrameId], 1) < 1)
  {
    return;
  }

  markChanged();

  paintPictureCurrentPart();
  paintCurrentFrameToResultRect();
}


void AnimFrameTool::moveFrameContentDown()
{
  if(m_pAnimSheets == NULL)
  {
    return;
  }

  // Move object (non-zero pixels) of current frame left by 1 pixel
  if(m_pBitMapTools->MoveObjectDown(m_FrameRects[m_FrameId], 1) < 1)
  {
    return;
  }

  markChanged();

  paintPictureCurrentPart();
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


void AnimFrameTool::openAnim()
{
  AslFileRequest request(m_pControlWindow);
  std::string filename = request.SelectFile("Open .iff/.ilbm or .abk file", 
                                            "", 
                                            false);
  std::string msgString;
  if(filename.length() > 0)
  {
    try
    {
      // Load the IFF ILBM picture (or AMOS sprite bank) and create a 
      // container of AnimSheets of it
      AnimSheetContainer* pNewSheetContainer;
      pNewSheetContainer = new AnimSheetContainer(filename.c_str());

      // New SheetContainer created sucessfully. Now Replace the
      // formerly laoded one with the new one.
      if(m_pAnimSheets != NULL)
      {
        delete m_pAnimSheets;
      }

      m_pAnimSheets = pNewSheetContainer;

      // Select the first sheet. Every instance of AnimSheetContainer
      // should contain a fisrt sheet.
      selectAnimSheet(0);

      GT_SetGadgetAttrs(m_pGadTxtFilename, m_pControlWindow, NULL,
                        GTTX_Text, m_Filename.c_str(),
                        TAG_DONE);

      GT_SetGadgetAttrs(m_pGadLvSheet, m_pControlWindow, NULL,
                        GTLV_Labels, m_pAnimSheets->getSheetList(),
                        GTLV_Selected, 0,
                        TAG_DONE);

      m_Filename = filename;
      m_HasChanged = false;

    }
    catch(const char* pMsg)
    {
      msgString = "Failed to load file\n  '";
      msgString += filename;
      msgString += "'\nas IFF / ILBM or AMOS .abk\n\n";
      msgString += pMsg;

      MessageBox request(m_pControlWindow);
      request.Show("Open error",
                   msgString.c_str(),
                   "Ok");
    }
  }
}

void AnimFrameTool::selectAnimSheet(ULONG index)
{
  if(m_pAnimSheets == NULL)
  {
    return;
  }

  if(index >= m_pAnimSheets->getNumSheets())
  {
    // Index to big
    return;
  }

  // Get the sheet item addressed by given index
  struct SheetItemNode* pSheetNode = m_pAnimSheets->getSheetItem(index);
  if(pSheetNode == NULL)
  {
    // Failed to get indexed Sheet item
    return;
  }

  if(m_pBitMapTools != NULL)
  {
    delete m_pBitMapTools;
  }

  // Create BitMapTools for this sheet. Needed for picture processing.
  m_pBitMapTools = new BitMapTools(pSheetNode->pBitMap);

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


void AnimFrameTool::saveIlbm()
{
  std::string msgString;
  std::size_t finalPointIdx = m_Filename.find_last_of('.');
  std::string saveFileName;

  if(finalPointIdx != std::string::npos)
  {
    saveFileName = m_Filename.substr(0, finalPointIdx);
    saveFileName += "_out";
    saveFileName += m_Filename.substr(finalPointIdx, std::string::npos);
  }
  else
  {
    saveFileName = m_Filename + "_out.iff";
  }

  try
  {
    // Save the anim picture (or AMOS .abk)
    // Throws an exception if it fails
    m_pAnimSheets->save(saveFileName.c_str());

    m_HasChanged = false;
    disableMenuItem(MID_ProjectSave);
  }
  catch(const char* pMsg)
  {
    msgString = "Failed to to save \n  '";
    msgString += saveFileName;
    msgString += "'\n\n";
    msgString += pMsg;

    MessageBox request(m_pControlWindow);
    request.Show("Save error",
                 msgString.c_str(),
                 "Ok");
  }
}


void AnimFrameTool::calcFrameRects()
{
  if(m_pAnimSheets == NULL)
  {
    return;
  }

  m_FrameId = 0;

  LONG selectedFrameWordWidth;
  if(1 != GT_GetGadgetAttrs(m_pGadSliFrameWidth, 
                            m_pControlWindow, 
                            NULL,
                            GTSL_Level, &selectedFrameWordWidth,
                            TAG_DONE))
  {
    return;
  }

  int selectedFrameWidth = selectedFrameWordWidth * 16;
  if(selectedFrameWidth == 0)
  {
    return;
  }

  m_FrameRects.clear();
  
  // Create the needed number of yet uninitialized Rects
  m_SheetNumFrames = m_pAnimSheets->getSheetItem(m_SheedId)->Width 
                   / selectedFrameWidth;

  for(ULONG i = 0; i < m_SheetNumFrames; i++)
  {
    m_FrameRects.push_back(Rect());
  }

  // Initialize the rects
  for(ULONG i = 0; i < m_SheetNumFrames; i++)
  {
    m_FrameRects[i].Set(i * selectedFrameWidth,                               // left
                        0,                                                    // top
                        ((i+1) * selectedFrameWidth) - 1,                     // right
                        m_pAnimSheets->getSheetItem(m_SheedId)->Height - 1);  // bottom
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
  sprintf(buf, "%d", m_SheetNumFrames);
  GT_SetGadgetAttrs(m_pGadTxtNumFrames, m_pControlWindow, NULL,
                    GTTX_Text, buf,
                    GTTX_CopyText, TRUE,
                    TAG_DONE);
}

void AnimFrameTool::paintPicture()
{
  if(m_pAnimSheets == NULL || m_pCanvasWindow == NULL)
  {
    return;
  }

  SetRast(m_pCanvasWindow->RPort, 0);

  BltBitMapRastPort(m_pAnimSheets->getSheetItem(m_SheedId)->pBitMap, 
                    0, 0, 
                    &m_pCanvasScreen->RastPort, 
                    0, 0,
                    m_pAnimSheets->getSheetItem(m_SheedId)->Width, m_pAnimSheets->getSheetItem(m_SheedId)->Height, 
                    0xc0);

  WaitBlit();
}

void AnimFrameTool::paintPictureCurrentPart()
{
  if(m_pAnimSheets == NULL || m_pCanvasWindow == NULL)
  {
    return;
  }

  const Rect& rect = m_FrameRects[m_FrameId];

  BltBitMapRastPort(m_pAnimSheets->getSheetItem(m_SheedId)->pBitMap, 
                    rect.Left() + 1, rect.Top() + 1, 
                    &m_pCanvasScreen->RastPort, 
                    rect.Left() + 1, rect.Top() + 1,
                    rect.Width() - 2, rect.Height() - 2, 
                    0xc0);

  WaitBlit();
}

void AnimFrameTool::paintGrid()
{
  if(m_pAnimSheets == NULL || m_pCanvasWindow == NULL)
  {
    return;
  }

  // Set the pens for the grid
  m_NormalRectPen = 1;
  m_HighlightedRectPen = (1L << m_pAnimSheets->getSheetItem(m_SheedId)->Depth) - 1;

  // Draw all m_FrameRect's
  for(size_t i = 0; i < m_FrameRects.size(); i++)
  {
    // For current frame set a different pen color (the highest pen available)
    bool isHighlighted = ((size_t)i == m_FrameId ? true : false);
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
  if((m_pAnimSheets == NULL) || (m_pControlWindow == NULL))
  {
    return;
  }

  BltBitMapRastPort(m_pAnimSheets->getSheetItem(m_SheedId)->pBitMap, 
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

    case GID_LvSheet:
      {
        selectAnimSheet(code);
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
        openAnim();
        break;

      case MID_ProjectSave:
        saveIlbm();
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

      case MID_ToolsPrintFullMask:
        { // Print the mask of the whole animation strip image
          if(m_pAnimSheets == NULL)
          {
            break;
          }

          ShadowMask mask(m_pAnimSheets->getSheetItem(m_SheedId)->pBitMap);
          mask.Print();
          break;
        }

      case MID_ToolsPrintFrameMask:
        { // Print the mask of the current animation frame
          if(m_pAnimSheets == NULL)
          {
            break;
          }

          // Create a temporary BitMap for current frame
          struct BitMap* pTmpBm = AllocBitMap(m_FrameRects[m_FrameId].Width(),
                                              m_FrameRects[m_FrameId].Height(),
                                              m_pAnimSheets->getSheetItem(m_SheedId)->Depth,
                                              BMF_CLEAR,
                                              NULL);

          if(pTmpBm == NULL)
          {
            break;
          }

          // Copy the current frame image into the temporary BitMap
          BltBitMap(m_pAnimSheets->getSheetItem(m_SheedId)->pBitMap,
                    m_FrameRects[m_FrameId].Left(), 
                    m_FrameRects[m_FrameId].Top(),
                    pTmpBm,
                    0, 0, 
                    m_FrameRects[m_FrameId].Width(),
                    m_FrameRects[m_FrameId].Height(),
                    0Xc0,
                    0xff,
                    NULL);

          // Create and print shadow mask
          ShadowMask mask(pTmpBm);
          mask.Print();

          FreeBitMap(pTmpBm);
          break;
        }
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

  if(m_pAnimSheets != NULL)
  {
    delete m_pAnimSheets;
    m_pAnimSheets = NULL;
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

  if(m_pAnimSheets != NULL)
  {
    ULONG screenWidth = m_pAnimSheets->getSheetItem(m_SheedId)->Width;
    if(screenWidth < m_OScanWidth)
    {
      screenWidth = m_OScanWidth;
    }

    m_pCanvasScreen = OpenScreenTags(NULL,
                                     SA_AutoScroll, 1,
                                     SA_Colors32, m_pAnimSheets->getColors32(),
                                     SA_DisplayID, VIEW_MODE_ID,
                                     SA_Depth, m_pAnimSheets->getSheetItem(m_SheedId)->Depth,
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
  if(m_pAnimSheets != NULL)
  {
    GT_SetGadgetAttrs(m_pGadScrCanvasHScroll, m_pControlWindow, NULL,
                      GTSC_Top, 0,
                      GTSC_Total, m_pAnimSheets->getSheetItem(m_SheedId)->Width,
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


void AnimFrameTool::disableMenuItem(MenuId menuId)
{
  WORD menuNumber = 0;
  struct MenuItem* pFoundItem = findItemByUserData((APTR)menuId, menuNumber);
  if(pFoundItem == NULL)
  {
    return;
  }

  OffMenu(m_pControlWindow, menuNumber);
}


void AnimFrameTool::enableMenuItem(MenuId menuId)
{
  WORD menuNumber = 0;
  struct MenuItem* pFoundItem = findItemByUserData((APTR)menuId, menuNumber);
  if(pFoundItem == NULL)
  {
    return;
  }

  OnMenu(m_pControlWindow, menuNumber);
}


struct MenuItem* AnimFrameTool::findItemByUserData(APTR pUserDataToFind,
                                                   WORD& foundMenuNumber)
{
  if(m_pMenu == NULL)
  {
    return NULL;
  }

  foundMenuNumber = 0;

  struct Menu* pMenu = m_pMenu;
  struct MenuItem* pItem = pMenu->FirstItem;
  if(pItem == NULL)
  {
    return NULL;
  }

  int iMenu = 0;
  int iItem = 0;

  do
  {
    do
    {
      APTR pUserData = GTMENUITEM_USERDATA(pItem);
      if(pUserData == pUserDataToFind)
      {
        foundMenuNumber = FULLMENUNUM(iMenu, iItem, 0);
        return pItem;
      }

      pItem = pItem->NextItem;
      iItem++;
    }
    while(pItem != NULL);

    pMenu = pMenu->NextMenu;
    if(pMenu != NULL)
    {
      pItem = pMenu->FirstItem;
      iItem = 0;
      iMenu++;
    }
  }
  while(pItem != NULL);

  return NULL;
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

  ng.ng_LeftEdge = m_ControlsRect.Left() + UI_LABEL_WIDTH;
  ng.ng_TopEdge += UI_RASTER_HEIGHT;
  ng.ng_Width = m_ControlsRect.Width() - UI_LABEL_WIDTH - UI_RASTER_WIDTH;
  ng.ng_Height = m_ControlsRect.Bottom() - ng.ng_TopEdge;
  ng.ng_GadgetID = GID_LvSheet;
  ng.ng_Flags = NG_HIGHLABEL|PLACETEXT_LEFT;
  ng.ng_GadgetText = (UBYTE*) "Anim sheet:";

  m_pGadLvSheet = pGadget = CreateGadget(LISTVIEW_KIND, pGadget, &ng,
                                         GTLV_ShowSelected, NULL,
                                         TAG_DONE);

  ng.ng_LeftEdge = 0;
  ng.ng_TopEdge = m_pControlScreen->Height - CANVAS_HEIGHT - rowHeight - 3; // Bottom of control screen
  ng.ng_Width = m_OScanWidth;
  ng.ng_Height = rowHeight;
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
