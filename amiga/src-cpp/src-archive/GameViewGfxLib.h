#ifndef GAME_VIEW_GFX_LIB_H
#define GAME_VIEW_GFX_LIB_H

#include <graphics/view.h>

#include "GameColors.h"
#include "GameViewBase.h"

/**
 * Represents an advanced view for games. It is constructed manually
 * using graphics.library functions of Amiga OS 2.0 and may in future
 * be enhanced by dual playfield capabilities.
 *
 * @author Uwe Rosner
 * @date 29/07/2019
 */
class GameViewGfxLib : public GameViewBase
{
public:
  GameViewGfxLib(long width, long height, short depth);
  ~GameViewGfxLib();

  bool Open(GameColors& colors);
  void Close();

  struct RastPort* RastPort();
  struct ViewPort* ViewPort();

  void Render();

  const char* LastError() const;
  const char* ViewName() const;


private:
  struct View* m_pOldView;

  struct View m_View;
  struct ViewPort* m_pViewPort;
  struct RastPort m_RastPort;
  struct DimensionInfo m_DimensionInfo;

  struct ColorMap* m_pColorMap;
  struct ViewExtra* m_pViewExtra;
  struct MonitorSpec* m_pMonitorSpec;
  struct ViewPortExtra* m_pViewPortExtra;

  struct BitMap* m_pBitMapArray[2];
  WORD m_FrameToggle;

  enum InitError
  {
    IE_None,
    IE_AlreadyInitialized,
    IE_GettingViewExtra,
    IE_GettingMonSpec,
    IE_GettingBitMapMem,
    IE_GettingBitPlanes,
    IE_GettingViewPort,
    IE_GettingVPExtra,
    IE_GettingDimInfo,
    IE_GettingDisplayInfo,
    IE_GettingCM,
    IE_AttachExtStructs,
  };

  InitError m_InitError;

};

#endif
