#ifndef I_GAME_VIEW_H
#define I_GAME_VIEW_H

#include <graphics/gfx.h>
#include <graphics/view.h>


/**
 * Represents an Interface for all supported GameViews.
 *
 * All GameViews are double buffered.
 *
 *  A GameView can be opened and must be closed when finished.
 *
 * The Render() method should be called every frame to draw the changed
 * scenery and moved objects.
 *
 * They contain a RastPort, ViewPort and View to allow graphics
 * operations to be performed.
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
  virtual bool Open() = 0;
  virtual void Close() = 0;

  virtual short Width() = 0;
  virtual short Height() = 0;
  virtual short Depth() = 0;

  virtual struct RastPort* RastPort() = 0;
  virtual struct ViewPort* ViewPort() = 0;
  virtual struct View* View() = 0;

  virtual void Render() = 0;

  virtual const char* LastError() const = 0;
};

#endif
