#ifndef ANIM_FRAME_TOOL_H
#define ANIM_FRAME_TOOL_H

#include <exec/ports.h>
#include <intuition/intuition.h>


class AnimFrameTool
{
public:
  AnimFrameTool();
  virtual ~AnimFrameTool();

  void Run();

private:
  void cleanup();
  struct Gadget* createAllGadgets(struct Gadget **ppGadgetList, 
                                  APTR pVisualInfo);

  /** 
   * Handle Intuition messages 
   */                         
  BOOL handleIntuiMessage(struct IntuiMessage* pIntuiMsg);

  void handleDBufMessage(struct Message* pDBufMsg);
  ULONG handleBufferSwap();
  struct BitMap* makeImageBM();

  /**
   * Close an Intuition window that shares a port with other Intuition
   * windows or IPC customers.
   *
   * We are careful to set the UserPort to null before closing, and to
   * free any messages that it might have been sent.
   */
  void CloseWindowSafely(struct Window* pIntuiWindow);

  /**
   * Remove and reply all IntuiMessages on a port that have been sent to
   * a particular window ( note that we don't rely on the ln_Succ
   * pointer of a message after we have replied it )
   */
  void StripIntuiMessages(struct MsgPort* pMsgPort, 
                          struct Window* pIntuiWindow);

  enum GadgetId
  {
    GID_HSlide,
    GID_VSlide
  };

  enum MenuId
  {
    MID_Run,
    MID_Step,
    MID_Quit,
    MID_HSlow,
    MID_HFast,
    MID_VSlow,
    MID_VFast
  };

  struct Screen* m_pCanvasScreen;
  struct Screen* m_pControlScreen;
  struct Window* m_pCanvasWindow;
  struct Window* m_pControlWindow;
  struct Gadget* m_pGadgetList;
  struct Gadget* m_pGadgetSlideHorizontal;
  struct Gadget* m_pGadgetSlideVertical;
  struct Menu* m_pMenu;
  APTR m_pVisualInfoCanvas;
  APTR m_pVisualInfoControl;

  struct MsgPort* m_pDBufPort;
  struct MsgPort* m_pUserPort;

  struct ScreenBuffer* m_pScreenBuffers[2];
  struct RastPort m_RastPorts[2];

  ULONG m_Status[2];

  ULONG m_BufCurrent;
  ULONG m_BufNextdraw;
  ULONG m_BufNextswap;
  ULONG m_Count;
};

#endif
