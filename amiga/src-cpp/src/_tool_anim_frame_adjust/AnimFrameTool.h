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

  struct Screen* m_pCanvasScreen = NULL;
  struct Screen* m_pControlScreen = NULL;
  struct Window* m_pCanvasWindow = NULL;
  struct Window* m_pControlWindow = NULL;
  struct Gadget* m_pGadgetList = NULL;
  struct Gadget* m_pGadgetSlideHorizontal;
  struct Gadget* m_pGadgetSlideVertical;
  struct Menu* m_pMenu = NULL;
  APTR m_pVisualInfoCanvas = NULL;
  APTR m_pVisualInfoControl = NULL;

  struct MsgPort* m_pDBufPort = NULL;
  struct MsgPort* m_pUserPort = NULL;

  struct ScreenBuffer* m_pScreenBuffers[2];
  struct RastPort m_RastPorts[2];

  ULONG m_Status[2];
};

#endif
