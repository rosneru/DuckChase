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


/**
 * Represents a graphics.library constructed, double buffered view
 * to be used for gaming purposes etc.
 *
 * @author Uwe Rosner
 * @date 29/07/2019
 */
class GameViewAdvanced
{
public:
  GameViewAdvanced(short viewWidth, short viewHeight, short viewDepth);
  ~GameViewAdvanced();

  bool Init();
  void FreeAll();

  struct RastPort* GetRastPort();
  struct ViewPort* GetViewPort();
  struct View* GetView();

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
