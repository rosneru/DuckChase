#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <graphics/gfxbase.h>

#include <stdio.h>
#include <string.h>

#include "PointsDisplay.h"

extern struct GfxBase* GfxBase;


PointsDisplay::PointsDisplay(IGameView& gameView,
                             short backPen, 
                             short strikesPen,
                             short fpsPen)
  : m_GameView(gameView),
    m_pRastPort(gameView.RastPort()),
    m_ViewWidth(gameView.Width()),
    m_ViewHeight(gameView.Height()),
    m_BackPen(backPen),
    m_StrikesPen(strikesPen),
    m_FpsPen(fpsPen)
{
  // Prepare the points display by filling a black rect at bottom
  SetAPen(m_pRastPort, m_BackPen);
  RectFill(m_pRastPort, 
           0,
           m_ViewHeight - 10,
           m_ViewWidth - 1,
           m_ViewHeight - 1);
}


PointsDisplay::~PointsDisplay()
{

}


void PointsDisplay::UpdateStrikes(short strikes)
{

}


void PointsDisplay::UpdateFps(short fps)
{
  char fpsBuf[16];
  snprintf(fpsBuf, 16, "FPS: %d", fps);

  SetAPen(m_pRastPort, m_BackPen);
  RectFill(m_pRastPort,
           m_ViewWidth - 90, 
           m_ViewHeight - 10, 
           m_ViewWidth - 1,
           m_ViewHeight - 1);
  
  Move(m_pRastPort, m_ViewWidth - 90, m_ViewHeight - 1 - 1);
  SetAPen(m_pRastPort, m_FpsPen);
  Text(m_pRastPort, fpsBuf, strlen(fpsBuf));
}
