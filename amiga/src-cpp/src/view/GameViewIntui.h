#ifndef GAME_VIEW_INTUI_20__H
#define GAME_VIEW_INTUI_20__H

#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <intuition/screens.h>

#include "IlbmBitmap.h"
#include "GameViewBase.h"

/**
 * Represents an view for games using the Intuition interface of
 * Amiga OS3.1+. It uses a intuition screen with two (double buffering)
 * custom bitmaps which are set up manually.
 *
 * @author Uwe Rosner
 * @date 31/07/2019
 */
class GameViewIntui : public GameViewBase
{
public:
  /**
   * Opens a game view (screen and transparent window) in an Intuition
   * compatible way.
   *
   * @param backgroundPicture This picture will be displayed as soon as
   * the view is opened. The view dimensions and the color palette are
   * taken from the picture.
   *
   */
  GameViewIntui(IlbmBitmap& backgroundPicture);
  virtual ~GameViewIntui();

  virtual struct RastPort* RastPort();
  virtual struct ViewPort* ViewPort();

  void Render();

  const char* ViewName() const;


private:
  struct Screen* m_pScreen;
  struct BitMap* m_pEmptyBitmap;
  void* m_pEmptyPointer;
  struct Window* m_pWindow;
  struct DBufInfo* m_pDBufInfo;
};

#endif
