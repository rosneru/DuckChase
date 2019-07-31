#ifndef GAME_VIEW_ADVANCED_H
#define GAME_VIEW_ADVANCED_H

#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <graphics/copper.h>
#include <graphics/view.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxnodes.h>
#include <graphics/videocontrol.h>

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

  void SwitchBuffers();

  const char* LastError() const;


private:
  short m_ViewWidth;
  short m_ViewHeight;
  short m_ViewDepth;
  short m_ViewNumColors;

  struct View* oldview; /// Pointer to old View we can restore it

  // Basic structures as of Release 1.3
  struct View view;
  struct ViewPort viewPort;
  struct BitMap bitMap1;
  struct BitMap bitMap2;
  struct ColorMap* cm;

  struct RastPort rastPort;

  // Extended structures used in Release 2
  struct ViewExtra* vextra;
  struct MonitorSpec* monspec;
  struct ViewPortExtra* vpextra;
  struct DimensionInfo dimquery;

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
