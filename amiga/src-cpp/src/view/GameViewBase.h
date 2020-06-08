#ifndef I_GAME_VIEW_H
#define I_GAME_VIEW_H

#include <exec/ports.h>
#include <graphics/gfx.h>
#include <graphics/view.h>

#include "IlbmBitmap.h"

/**
 * Represents the base class for all supported GameViews.
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
class GameViewBase
{
public:
  /**
   * Returns the background picture associated with this game view.
   */
  IlbmBitmap& BackgroundPicture();


  long Width();
  long WordWidth();
  long Height();
  long Depth();

  void SetPlayfieldBorders(long top, long left, long bottom, long right);

  long BorderTop();
  long BorderLeft();
  long BorderRight();
  long BorderBottom();


  //
  // The following methods must be implemented by inheriting classes.
  //

  virtual struct RastPort* RastPort() = 0;
  virtual struct ViewPort* ViewPort() = 0;

  virtual void Render() = 0;

  virtual const char* ViewName() const = 0;


protected:
  IlbmBitmap& m_BackgroundPicture;
  struct BitMap* m_pBitMapArray[2];

  short m_BorderTop;
  short m_BorderLeft;
  short m_BorderBottom;
  short m_BorderRight;

  // Double buffering
  bool m_bDBufSafeToChange;
  bool m_bDBufSafeToWrite;
  int m_CurrentBuf;

  /**
   * Replied to when safe to write to old BitMap
   */
  struct MsgPort* m_pSafeMessage;

  /**
   * Replied to when new BitMap has been displayed at least once
   */
  struct MsgPort* m_pDispMessage;

  GameViewBase(IlbmBitmap& backgroundPicture);
  virtual ~GameViewBase();
};

#endif
