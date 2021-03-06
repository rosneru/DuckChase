#ifndef LOWLEVELVIEW_H
#define LOWLEVELVIEW_H

#include <graphics/view.h>

/**
 * Represents a low level view object. Adapted from the example
 * LowLevelView.c as described in the Amiga CD32 Developer Manual.
 *
 * @author Uwe Rosner
 * @date 27/08/2019
 */
class LowlevelView
{
public:
  LowlevelView(ULONG modeId);
  virtual ~LowlevelView();

  struct View* View();

private:
  struct View* m_pView;

};

#endif
