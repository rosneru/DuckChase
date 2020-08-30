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
};

#endif
