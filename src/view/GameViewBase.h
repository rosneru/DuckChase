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
   * Display the given picture at given position.
   */
  void BlitPicture(const IlbmBitmap& picture, ULONG left, ULONG top);

  /**
   * Displaysthe given picture centered.
   */
  void BlitPicture(const IlbmBitmap& picture);

  /**
   * Display the given picture at given position. Using the mask for
   * display, so pixels with color 0 will be transparent.
   */
  void BlitPictureMasked(IlbmBitmap& picture, ULONG left, ULONG top);

  /**
   * Display the given picture centered. Using the mask for
   * display, so pixels with color 0 will be transparent.
   */
  void BlitPictureMasked(IlbmBitmap& picture);

  void SetPlayfieldBorders(long top, long left, long bottom, long right);

  long Width() const;
  long WordWidth() const;
  long Height() const;
  long Depth() const;

  long BorderTop() const;
  long BorderLeft() const;
  long BorderRight() const;
  long BorderBottom() const;


  //
  // The following methods must be implemented by inheriting classes.
  //

  virtual struct RastPort* RastPort() = 0;
  virtual struct ViewPort* ViewPort() = 0;

  virtual void Render() = 0;

  virtual const char* ViewName() const = 0;


protected:
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

  GameViewBase(IlbmBitmap& picture);
  virtual ~GameViewBase();

private:
  ULONG m_Width;
  ULONG m_WordWidth;
  ULONG m_Height;
  ULONG m_Depth;
};

#endif
