#ifndef I_GAME_VIEW_H
#define I_GAME_VIEW_H

#include <graphics/gfx.h>
#include <graphics/view.h>


/**
 * Represents an Interface for all supported GameViews.
 *
 * All GameViews are double buffered. They contain a method
 * SwitchBuffers() to switch between the displayed and the to be drawn
 * image.
 * 
 * GameViews can be opened and must be closed when finished.
 *
 * They contain a RastPort, ViewPort and View to allow graphics 
 * operations to be performed.
 *
 * Finally they contain a LastError method, reporting a text of what 
 * has gone, which currently only is set when Open() fails.
 *
 *
 * @author Uwe Rosner
 * @date 31/07/2019
 */
class IGameView
{
public:
  bool Open() = 0;
  void Close() = 0;

  struct RastPort* RastPort() = 0;
  struct ViewPort* ViewPort() = 0;
  struct View* View() = 0;

  void SwitchBuffers() = 0;

  const char* LastError() const = 0;
};

#endif
