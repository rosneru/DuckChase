#ifndef ANIM_FRAME_TOOL_H
#define ANIM_FRAME_TOOL_H

#include <exec/ports.h>
#include <intuition/intuition.h>

#include <vector>
#include <string>

#include "AnimSheetContainer.h"
#include "BitMapTools.h"
#include "Rect.h"



/**
 * Anim Sheet Adjust
 * Adjust anim frames in IFF ILBM or AMOS sprite banks (abk) files
 *
 * A tool to adjust frames of an anim strip on a horizontal row to a
 * raster.
 * 
 * Supported file formats for the anim strip are IFF ILBM and AMOS ABK.
 *
 * Rough idea write up done on 26.08.2020 in Xertigny.
 *
 * @author Uwe Rosner
 * @date 29/08/2020
 */
class AnimFrameTool
{
public:
  AnimFrameTool();
  virtual ~AnimFrameTool();

  void Run();

private:
  ULONG m_OScanWidth;
  ULONG m_OScanHeight;
  AnimSheetContainer* m_pAnimSheets;
  BitMapTools* m_pBitMapTools;
  ULONG m_NormalRectPen;
  ULONG m_HighlightedRectPen;

  bool m_HasChanged;
  
  ULONG m_SheetId;
  ULONG m_SheetNumFrames;
  ULONG m_FrameId;

  enum GadgetId
  {
    GID_ScrCanvasHoriz,
    GID_TxtFilename,
    GID_SliFrameWordWidth,
    GID_LvSheet,
    GID_BtnStop,
    GID_IntCurrentFrame,
    GID_TxtNumFrames
  };

  enum MenuId
  {
    MID_ProjectOpenAnim,
    MID_ProjectSave,
    MID_ProjectSaveAs,
    MID_ProjectExportToAbk,
    MID_ProjectExportToIlbm,
    MID_ProjectAbout,
    MID_ProjectQuit,
    MID_EditAppendSheet,
    MID_ToolsCenterAllFrames,
    MID_ToolsGetMaxWidth,
    MID_ToolsPrintFullMask,
    MID_ToolsPrintFrameMask,
    MID_ToolsPrintInterleavedFrameMask
  };

  struct Screen* m_pCanvasScreen;
  struct Screen* m_pControlScreen;
  struct Window* m_pCanvasWindow;
  struct Window* m_pControlWindow;

  struct Gadget* m_pGadgetList;
  struct Gadget* m_pGadScrCanvasHScroll;
  struct Gadget* m_pGadTxtFilename;
  struct Gadget* m_pGadSliFrameWidth;
  struct Gadget* m_pGadLvSheet;
  struct Gadget* m_pGadIntCurrentFrame;
  struct Gadget* m_pGadTxtNumFrames;

  struct Menu* m_pMenu;
  APTR m_pVisualInfoCanvas;
  APTR m_pVisualInfoControl;

  struct MsgPort* m_pUserPort;

  Rect m_ResultFrameRect;
  Rect m_ControlsRect;

  std::vector<Rect> m_FrameRects;

  void markChanged();

  void moveFrameContentLeft();
  void moveFrameContentRight();
  void moveFrameContentUp();
  void moveFrameContentDown();

  void gadgetFrameWidthChanged();
  void gadgetCurrentFrameChanged();

  void selectPreviousFrame();
  void selectNextFrame();

  void selectAnimSheet(ULONG index);
  
  void open();
  void save();
  void saveAs();
  void exportToAbk();
  void exportToIlbm();
  void about();
  bool quit();

  void appendSheet();

  std::string askSaveFilename(std::string requestTitle);

  bool askContinueIfChanged(std::string continueActionText, 
                            std::string continueButtonText);

  void calcFrameRects(LONG selectedFrameWordWidth);
  void updateFrameIdxGadgets(bool bCurrentOnly);

  void paintPicture();
  void paintPictureCurrentPart();
  void paintGrid();
  void paintSelectionRect(const Rect& rect, bool isHighlighted);
  void paintCurrentFrameToResultRect();

  /** 
   * Handle Intuition messages 
   */                         
  bool handleIntuiMessage(struct IntuiMessage* pIntuiMsg);

  void cleanup();

  void openCanvas();
  void closeCanvas();

  /**
   * Disable menu item with given id.
   * 
   * Browses the menu associated to the given window for an item which
   * contains the given menuId in field userdata. If such an menu item
   * is found, it is disabled.
   *
   * NOTE At the moment subitems are skipped and won't be disabled.
   */
  void disableMenuItem(MenuId menuId);

  /**
   * Enable menu item with given id.
   * 
   * Browses the menu associated to the given window for an item which
   * contains the given menuId in field userdata. If such an menu item
   * is found, it is enabled.
   *
   * NOTE At the moment subitems are skipped and won't be disabled.
   */
  void enableMenuItem(MenuId menuId);
  /**
   * Browses all menu items trying to find the item with the given
   * userdata
   *
   * @param
   *
   * NOTE At the moment subitems are skipped
   */
  struct MenuItem* findItemByUserData(APTR pUserDataToFind,
                                      WORD& foundMenuNumber);

  struct Gadget* createGadgets(struct Gadget **ppGadgetList, 
                               APTR pVisualInfo);
  
  /**
   * Close an Intuition window that shares a port with other Intuition
   * windows or IPC customers.
   *
   * We are careful to set the UserPort to null before closing, and to
   * free any messages that it might have been sent.
   */
  void closeWindowSafely(struct Window* pIntuiWindow);

  /**
   * Remove and reply all IntuiMessages on a port that have been sent to
   * a particular window ( note that we don't rely on the ln_Succ
   * pointer of a message after we have replied it )
   */
  void stripIntuiMessages(struct MsgPort* pMsgPort, 
                          struct Window* pIntuiWindow);
};

#endif
