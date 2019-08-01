#ifndef GAME_VIEW_SIMPLE_H
#define GAME_VIEW_SIMPLE_H

#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <intuition/screens.h>

#include "IGameView.h"

/**
 * Represents a simple view for games. It uses a intuition screen with 
 * two (double buffering) custom bitmaps which are set up manually.
 *
 * @author Uwe Rosner
 * @date 31/07/2019
 */
class GameViewSimple : public IGameView
{
public:
  GameViewSimple(short viewWidth, short viewHeight, short viewDepth);
  ~GameViewSimple();

  bool Open();
  void Close();

  short Width();
  short Height();

  struct RastPort* RastPort();
  struct ViewPort* ViewPort();
  struct View* View();

  void Render();

  const char* LastError() const;


private:
  short m_ViewWidth;
  short m_ViewHeight;
  short m_ViewDepth;
  short m_ViewNumColors;

  struct Screen* m_pScreen;

  struct BitMap m_BitMap1;
  struct BitMap m_BitMap2;

  enum InitError
  {
    IE_None,
    IE_AlreadyInitialized,
    IE_GettingBitPlanes,
    IE_OpeningScreen,
  };

  InitError m_InitError;
  bool m_BufToggle;

};

#endif
