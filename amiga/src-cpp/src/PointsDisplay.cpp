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
                             short fpsPen,
                             short infoPen)
  : m_GameView(gameView),
    m_ViewWidth(gameView.Width()),
    m_ViewHeight(gameView.Height()),
    m_BackPen(backPen),
    m_StrikesPen(strikesPen),
    m_FpsPen(fpsPen),
    m_InfoPen(infoPen)
{

}


PointsDisplay::~PointsDisplay()
{

}

void PointsDisplay::Clear()
{
  if(m_GameView.RastPort() == NULL)
  {
    return;
  }


  // Drawing a filled black rect at the bottom of the view
  SetAPen(m_GameView.RastPort(), m_BackPen);
  SetBPen(m_GameView.RastPort(), m_BackPen);
  RectFill(m_GameView.RastPort(),
           0,
           m_ViewHeight - 12,
           m_ViewWidth - 1,
           m_ViewHeight - 1);
}


void PointsDisplay::UpdateStrikes(short strikes)
{
  if(m_GameView.RastPort() == NULL)
  {
    return;
  }
}


void PointsDisplay::UpdateFps(short fps)
{
  if(m_GameView.RastPort() == NULL)
  {
    return;
  }

  char fpsBuf[16];
  sprintf(fpsBuf, "FPS: %d", fps);

  SetAPen(m_GameView.RastPort(), m_BackPen);
  SetBPen(m_GameView.RastPort(), m_BackPen);
  RectFill(m_GameView.RastPort(),
           m_ViewWidth - 90,
           m_ViewHeight - 12,
           m_ViewWidth - 4,
           m_ViewHeight - 2);

  Move(m_GameView.RastPort(), m_ViewWidth - 90, m_ViewHeight - 2 - 1);
  SetAPen(m_GameView.RastPort(), m_FpsPen);
  Text(m_GameView.RastPort(), fpsBuf, strlen(fpsBuf));
}


void PointsDisplay::UpdateInfo(const char* pInfo)
{
  if(m_GameView.RastPort() == NULL)
  {
    return;
  }

  SetAPen(m_GameView.RastPort(), m_BackPen);
  SetBPen(m_GameView.RastPort(), m_BackPen);
  RectFill(m_GameView.RastPort(),
           5,
           m_ViewHeight - 12,
           90,
           m_ViewHeight - 2);

  Move(m_GameView.RastPort(), 5, m_ViewHeight - 2 - 1);
  SetAPen(m_GameView.RastPort(), m_InfoPen);
  Text(m_GameView.RastPort(), pInfo, strlen(pInfo));
}

