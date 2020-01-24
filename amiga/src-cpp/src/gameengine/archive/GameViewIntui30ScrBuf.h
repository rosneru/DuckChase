#ifndef GAME_VIEW_INTUI_30_SCRBUF_H
#define GAME_VIEW_INTUI_30_SCRBUF_H

#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <intuition/screens.h>

#include "GameColors.h"
#include "IGameView.h"

#define	OK_REDRAW	1	/* Buffer fully detached, ready for redraw */
#define OK_SWAPIN	2	/* Buffer redrawn, ready for swap-in */

/**
 * Represents an view for games using the Intuition interface of
 * Amiga OS3.0.
 *
 * NOTE: This seems not to meet my current needs. It provides double 
 *       buffering for a screen buf. It switches the screen buf and the 
 *       active RastPort. The problem when using GELS is that these are 
 *       drawn automatically into the RastPort for which they have been
 *       created in. And this is mostly *not* the current / switched
 *       RastPort.
 *
 * @author Uwe Rosner
 * @date 13/08/2019
 */
class GameViewIntui30ScrBuf : public IGameView
{
public:
  GameViewIntui30ScrBuf(short viewWidth, short viewHeight, short viewDepth);
  ~GameViewIntui30ScrBuf();

  bool Open(GameColors& colors);
  void Close();

  short Width();
  short Height();
  short Depth();

  struct RastPort* RastPort();
  struct RastPort* RastPort1();
  struct RastPort* RastPort2();
  struct ViewPort* ViewPort();

  void Render();

  const char* LastError() const;
  const char* ViewName() const;


private:
  short m_ViewWidth;
  short m_ViewHeight;
  short m_ViewDepth;
  short m_ViewNumColors;

  struct MsgPort* dbufport;

  struct ScreenBuffer *scbuf[2];

  struct RastPort rport[2];

  ULONG sigs;

  ULONG buf_current;
  ULONG buf_nextdraw;
  ULONG buf_nextswap;

  struct Screen* m_pScreen;

  ULONG status[2];

  enum InitError
  {
    IE_None,
    IE_AlreadyInitialized,
    IE_CreateMsgPort,
    IE_OpeningScreen,
    IE_AllocScreenBuf1,
    IE_AllocScreenBuf2,
  };

  InitError m_InitError;

  /**
   * Handle the rendering and swapping of the buffers
   */
  ULONG handleBufferSwap();

  /**
   *
   */
  void handleDBufMessage(struct Message *dbmsg);

};

#endif
