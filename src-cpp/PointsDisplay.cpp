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
    m_pRastPort(NULL),
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
  if(m_pRastPort == NULL)
  {
    if(m_GameView.RastPort() == NULL)
    {
      return;
    }

    m_pRastPort = m_GameView.RastPort();
  }

  // Drawing a filled black rect at the bottom of the view
  SetAPen(m_pRastPort, m_BackPen);
  SetBPen(m_pRastPort, m_BackPen);
  RectFill(m_pRastPort,
           0,
           m_ViewHeight - 10,
           m_ViewWidth - 1,
           m_ViewHeight - 1);
}


void PointsDisplay::UpdateStrikes(short strikes)
{
  if(m_pRastPort == NULL)
  {
    if(m_GameView.RastPort() == NULL)
    {
      return;
    }

    m_pRastPort = m_GameView.RastPort();
  }
}


void PointsDisplay::UpdateFps(short fps)
{
  if(m_pRastPort == NULL)
  {
    if(m_GameView.RastPort() == NULL)
    {
      return;
    }

    m_pRastPort = m_GameView.RastPort();
  }

  char fpsBuf[16];
  sprintf(fpsBuf, "FPS: %d", fps);

  SetAPen(m_pRastPort, m_BackPen);
  SetBPen(m_pRastPort, m_BackPen);
  RectFill(m_pRastPort,
           m_ViewWidth - 90,
           m_ViewHeight - 10,
           m_ViewWidth - 1,
           m_ViewHeight - 1);

  Move(m_pRastPort, m_ViewWidth - 90, m_ViewHeight - 1 - 1);
  SetAPen(m_pRastPort, m_FpsPen);
  Text(m_pRastPort, fpsBuf, strlen(fpsBuf));
}

void PointsDisplay::UpdateInfo(const char* pInfo)
{
  if(m_pRastPort == NULL)
  {
    if(m_GameView.RastPort() == NULL)
    {
      return;
    }

    m_pRastPort = m_GameView.RastPort();
  }

  SetAPen(m_pRastPort, m_BackPen);
  SetBPen(m_pRastPort, m_BackPen);
  RectFill(m_pRastPort,
           2,
           m_ViewHeight - 10,
           90,
           m_ViewHeight - 1);

  Move(m_pRastPort, 2, m_ViewHeight - 1 - 1);
  SetAPen(m_pRastPort, m_InfoPen);
  size_t lelel = strlen(pInfo);
  Text(m_pRastPort, pInfo, lelel);
}
