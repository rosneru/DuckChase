#ifndef GAME_VIEW_LOWLEVEL_H
#define GAME_VIEW_LOWLEVEL_H

#include <graphics/rastport.h>
#include <graphics/view.h>

#include "lowlevelview.h"
#include "lowlevelviewport.h"

#include "IGameView.h"

/**
 * Represents an advanced view for games. It is constructed manually
 * using graphics.library functions and two sub classes as described
 * in the Amiga CD32 developers manual.
 *
 * @author Uwe Rosner
 * @date 29/07/2019
 */
class GameViewLowlevel : public IGameView
{
public:
  GameViewLowlevel(short viewWidth, short viewHeight, short viewDepth);
  ~GameViewLowlevel();

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
  char* m_pLastError;

  LowlevelView m_LowLevelView;
  LowlevelViewPort m_LowLevelViewPort;

  struct View* m_pOldView;

  struct View* m_pView;
  struct ViewPort* m_pViewPort;
  struct RastPort m_RastPort;

  struct BitMap* m_pBitMapArray[2];
  WORD m_FrameToggle;



};

#endif
