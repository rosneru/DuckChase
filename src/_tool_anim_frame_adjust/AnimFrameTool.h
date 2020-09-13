#ifndef ANIM_FRAME_TOOL_H
#define ANIM_FRAME_TOOL_H

#include <exec/ports.h>
#include <intuition/intuition.h>

#include <vector>
#include <string>

#include "IlbmBitmap.h"
#include "Rect.h"



/**
 * A tool to adjust frames of an anim strip on a horizontal row in an
 * iff ilbm file to a raster.
 *
 * Solution is based on the AmigaOS NDK3.1 DBuf example.
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
  std::string m_Filename;
  ULONG m_OScanWidth;
  ULONG m_OScanHeight;
  IlbmBitmap* m_pLoadedPicture;
  ULONG m_NormalRectPen;
  ULONG m_HighlightedRectPen;
  
  int m_NumFrames;
  int m_CurrentFrameIdx;

  enum GadgetId
  {
    GID_HScroll,
    GID_TextFilename,
    GID_SlideFrameWordWidth,
    GID_ButtonPlay,
    GID_ButtonStop,
    GID_StringCurrentFrame,
    GID_TextNumFrames
  };

  enum MenuId
  {
    MID_ProjectOpenAnim,
    MID_ProjectSaveAnim,
    MID_ProjectAbout,
    MID_ProjectQuit,
    MID_ToolsCenterAllFrames,
    MID_ToolsGetMaxWidth
  };

  struct Screen* m_pCanvasScreen;
  struct Screen* m_pControlScreen;
  struct Window* m_pCanvasWindow;
  struct Window* m_pControlWindow;

  struct Gadget* m_pGadgetList;
  struct Gadget* m_pGadgetHScroll;
  struct Gadget* m_pGadgetTextFilename;
  struct Gadget* m_pGadgetFrameWidth;
  struct Gadget* m_pGadgetButtonPlay;
  struct Gadget* m_pGadgetButtonStop;
  struct Gadget* m_pGadgetStringCurrFrame;
  struct Gadget* m_pGadgetTextNumFrames;

  struct Menu* m_pMenu;
  APTR m_pVisualInfoCanvas;
  APTR m_pVisualInfoControl;

  struct MsgPort* m_pUserPort;

  Rect m_ResultFrameRect;
  Rect m_ControlsRect;

  std::vector<Rect> m_FrameRects;

  void moveFrameContentLeft();
  void moveFrameContentRight();
  void moveFrameContentUpward();
  void moveFrameContentDownward();

  void selectPreviousFrame();
  void selectNextFrame();
  
  void openAnimIlbmPicture();

  void calcFrameRects();
  void updateFrameIdxGadgets(bool bCurrentOnly);

  void paintPicture();
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
