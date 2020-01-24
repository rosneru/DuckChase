#ifndef GAME_VIEW_LOWLEVEL_H
#define GAME_VIEW_LOWLEVEL_H

#include <graphics/rastport.h>
#include <graphics/view.h>

#include "lowlevelview.h"
#include "lowlevelviewport.h"
#include "GameColors.h"
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
  GameViewLowlevel(short width, 
                   short height, 
                   short depth, 
                   short numColors,
                   ULONG modeId);

  ~GameViewLowlevel();

  bool Open(GameColors& colors);
  void Close();

  short Width();
  short Height();
  short Depth();

  struct RastPort* RastPort();
  struct ViewPort* ViewPort();

  void Render();

  const char* LastError() const;
  const char* ViewName() const;


private:
  short m_Width;
  short m_Height;
  short m_Depth;
  short m_NumColors;
  ULONG m_ModeId;
  const char* m_pLastError;

  struct Screen* m_pDummyScreen;
  LowlevelView m_LowLevelView;
  LowlevelViewPort m_LowLevelViewPort;

  struct View* m_pOldView;

  struct View* m_pView;
  struct ViewPort* m_pViewPort;
  struct RastPort m_RastPort;

  WORD m_FrameToggle;
  struct BitMap* m_pBitMapArray[2];



};

#endif
