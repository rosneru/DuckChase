#ifndef GAME_VIEW_INTUI_30__H
#define GAME_VIEW_INTUI_30__H

#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <intuition/screens.h>

#include "IGameView.h"

#define	OK_REDRAW	1	/* Buffer fully detached, ready for redraw */
#define OK_SWAPIN	2	/* Buffer redrawn, ready for swap-in */

/**
 * Represents an view for games using the Intuition interface of 
 * Amiga OS3.0.
 *
 * @author Uwe Rosner
 * @date 13/08/2019
 */
class GameViewIntui30 : public IGameView
{
public:
  GameViewIntui30(short viewWidth, short viewHeight, short viewDepth);
  ~GameViewIntui30();

  bool Open();
  void Close();

  short Width();
  short Height();
  short Depth();

  struct RastPort* RastPort();
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

  ULONG count;

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
