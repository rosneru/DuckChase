#ifndef GAME_VIEW_ADVANCED_H
#define GAME_VIEW_ADVANCED_H

#include <graphics/view.h>


#include "IGameView.h"

/**
 * Represents an advanced view for games. It is constructed manually
 * using graphics.library functions and may in future be enhanced by
 * dual playfield capabilities.
 *
 * @author Uwe Rosner
 * @date 29/07/2019
 */
class GameViewAdvanced : public IGameView
{
public:
  GameViewAdvanced(short viewWidth, short viewHeight, short viewDepth);
  ~GameViewAdvanced();

  bool Open();
  void Close();

  struct RastPort* RastPort();
  struct ViewPort* ViewPort();
  struct View* View();

  void Render();

  const char* LastError() const;


private:
  short m_ViewWidth;
  short m_ViewHeight;
  short m_ViewDepth;
  short m_ViewNumColors;

  struct View* m_pOldView; // Pointer to old View, so we can restore it

  // Basic structures as of Release 1.3
  struct View m_View;
  struct ViewPort m_ViewPort;
  struct BitMap m_BitMap1;
  struct BitMap m_BitMap2;
  struct ColorMap* m_pColorMap;

  struct RastPort m_RastPort;

  // Extended structures used in Release 2
  struct ViewExtra* m_pViewExtra;
  struct MonitorSpec* m_pMonitorSpec;
  struct ViewPortExtra* m_pViewPortExtra;
  struct DimensionInfo m_DimensionInfo;

  enum InitError
  {
    IE_None,
    IE_AlreadyInitialized,
    IE_GettingViewExtra,
    IE_GettingMonSpec,
    IE_GettingBitPlanes,
    IE_GettingVPExtra,
    IE_GettingDimInfo,
    IE_GettingDisplayInfo,
    IE_GettingCM,
    IE_AttachExtStructs,
  };

  InitError m_InitError;
  bool m_BufToggle;

};

#endif
