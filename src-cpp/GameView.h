#ifndef GAME_VIEW_H
#define GAME_VIEW_H

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
class GameView
{
public:
  GameView(short viewWidth, short viewHeight, short viewDepth);
  ~GameView();
  
  bool Init();

private:
  short m_ViewWidth;
  short m_ViewHeight;
  short m_ViewDepth;

  // Basic structures as of Release 1.3
  struct View view;
  struct ViewPort viewPort;
  struct BitMap bitMap1;
  struct BitMap bitMap2;
  struct BitMap* pBitMap; // TODO remove from GameView to GameLoop ?
  struct ColorMap *cm = NULL;

  // Extended structures used in Release 2
  struct ViewExtra *vextra = NULL;
  struct MonitorSpec *monspec = NULL;
  struct ViewPortExtra *vpextra = NULL;
  struct DimensionInfo dimquery;
};

#endif
