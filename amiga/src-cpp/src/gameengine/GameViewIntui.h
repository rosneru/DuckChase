#ifndef GAME_VIEW_INTUI_20__H
#define GAME_VIEW_INTUI_20__H

#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <intuition/screens.h>

#include "IGameView.h"

/**
 * Represents an view for games using the Intuition interface of
 * Amiga OS2.0. It uses a intuition screen with two (double buffering)
 * custom bitmaps which are set up manually.
 *
 * @author Uwe Rosner
 * @date 31/07/2019
 */
class GameViewIntui : public IGameView
{
public:
  GameViewIntui(short viewWidth, short viewHeight, short viewDepth);
  ~GameViewIntui();

  bool Open();
  void Close();

  short Width();
  short Height();
  short Depth();

  struct RastPort* RastPort();
  struct ViewPort* ViewPort();

  void SetColor32(int i, int r, int g, int b);

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
  WORD m_FrameToggle;

  enum InitError
  {
    IE_None,
    IE_AlreadyInitialized,
    IE_GettingBitMapMem,
    IE_GettingInterleavedBitMap,
    IE_OpeningScreen,
  };

  InitError m_InitError;

};

#endif
