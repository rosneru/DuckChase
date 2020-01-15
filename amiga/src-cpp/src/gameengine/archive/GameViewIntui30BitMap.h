#ifndef GAME_VIEW_INTUI_30_BITMAP_H
#define GAME_VIEW_INTUI_30_BITMAP_H

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
 * NOTE: This seems not to meet my current needs. It provides double 
 *       buffering for screen by using two BitMaps. It switches the 
 *       BitMaps as it should and would I draw into the active one,
 *       all would be fine. But I'm using GELS which have been 
 *       initilized with a RastPort and I'm also writing text etc into
 *       it. And this seems not to work with this BitMap-switching 
 *       double buffering.
 *
 * @author Uwe Rosner
 * @date 14/08/2019
 */
class GameViewIntui30BitMap : public IGameView
{
public:
  GameViewIntui30BitMap(short viewWidth, short viewHeight, short viewDepth);
  ~GameViewIntui30BitMap();

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

  struct BitMap* BitMap1();
  struct BitMap* BitMap2();


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
