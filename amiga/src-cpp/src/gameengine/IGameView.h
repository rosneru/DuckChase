#ifndef I_GAME_VIEW_H
#define I_GAME_VIEW_H

#include <graphics/gfx.h>
#include <graphics/view.h>

#include "GameColors.h"

/**
 * Represents an Interface for all supported GameViews.
 *
 * All GameViews are double buffered.
 *
 * A GameView can be opened and must be closed when finished.
 *
 * The Render() method should be called every frame to draw the changed
 * scenery and moved objects.
 *
 * They contain a ViewPort and a RastPort to allow graphics operations
 * to be performed.
 *
 * Finally they contain a LastError method, reporting a text of what
 * has gone wrong, which currently only is set when Open() fails.
 *
 *
 * @author Uwe Rosner
 * @date 31/07/2019
 */
class IGameView
{
public:
  virtual bool Open(GameColors& colors) = 0;
  virtual void Close() = 0;

  /**
   * Disbles the double buffering for the game view.
   *
   * NOTE: Only intended for testing purposes. In implementations just
   * the machanism will be disabled, acquiring the buffer memory will be
   * done just like in double buffer mode.
   */
  virtual void DisableDoubleBuf() = 0;

  virtual short Width() = 0;
  virtual short Height() = 0;
  virtual short Depth() = 0;

  virtual struct RastPort* RastPort() = 0;
  virtual struct ViewPort* ViewPort() = 0;

  virtual void Render() = 0;

  virtual const char* LastError() const = 0;
  virtual const char* ViewName() const = 0;
};

#endif
