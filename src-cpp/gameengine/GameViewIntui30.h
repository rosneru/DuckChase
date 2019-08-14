#ifndef GAME_VIEW_INTUI_30_H
#define GAME_VIEW_INTUI_30_H

#include <exec/ports.h>
#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <intuition/screens.h>

#include "IGameView.h"

/**
 * Represents an view for games using the Intuition interface of
 * Amiga OS3.0.
 *
 * @author Uwe Rosner
 * @date 14/08/2019
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

  struct Screen* m_pScreen;

  struct BitMap* m_pBitMapArray[2];
  int m_CurrBuffer;

  bool m_bSafeToWrite;
  bool m_bSafeToChange;
  
  struct DBufInfo* m_pDBufInfo;
  struct MsgPort* m_pMsgPortArray[2];


  enum InitError
  {
    IE_None,
    IE_AlreadyInitialized,
    IE_CreatingMsgPort1,
    IE_CreatingMsgPort2,
    IE_GettingDBufInfo,
    IE_GettingBitMapMem,
    IE_GettingBitPlanes,
    IE_OpeningScreen,
  };

  InitError m_InitError;

};

#endif
