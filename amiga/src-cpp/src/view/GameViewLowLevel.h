#ifndef GAME_VIEW_LOWLEVEL_H
#define GAME_VIEW_LOWLEVEL_H

#include <graphics/rastport.h>
#include <graphics/view.h>

#include "lowlevelview.h"
#include "lowlevelviewport.h"
#include "IlbmBitMap.h"
#include "GameViewBase.h"

/**
 * Represents an advanced view for games. It is constructed manually
 * using graphics.library functions and two sub classes as described
 * in the Amiga CD32 developers manual.
 *
 * @author Uwe Rosner
 * @date 29/07/2019
 */
class GameViewLowlevel : public GameViewBase
{
public:
  GameViewLowlevel(IlbmBitMap& backgroundPicture,
                   ULONG modeId);

  virtual ~GameViewLowlevel();

  struct RastPort* RastPort();
  struct ViewPort* ViewPort();

  void Render();

  const char* ViewName() const;


private:
  LowlevelView m_LowLevelView;
  LowlevelViewPort m_LowLevelViewPort;
  struct RastPort m_RastPort;
  struct View* m_pOldView;
  struct DBufInfo* m_pDBufInfo;
};

#endif
