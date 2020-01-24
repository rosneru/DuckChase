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
  : m_View(gameView),
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
  if(m_View.RastPort() == NULL)
  {
    return;
  }


  // Drawing a filled black rect at the bottom of the view
  SetAPen(m_View.RastPort(), m_BackPen);
  SetBPen(m_View.RastPort(), m_BackPen);
  RectFill(m_View.RastPort(),
           0,
           m_View.Height() - 12,
           m_View.Width()- 1,
           m_View.Height() - 1);
}


void PointsDisplay::UpdateStrikes(short strikes)
{
  if(m_View.RastPort() == NULL)
  {
    return;
  }
}


void PointsDisplay::UpdateFps(short fps)
{
  char fpsBuf[16];
  sprintf(fpsBuf, "FPS: %d", fps);

  SetAPen(m_View.RastPort(), m_BackPen);
  SetBPen(m_View.RastPort(), m_BackPen);
  RectFill(m_View.RastPort(),
           m_View.Width()- 90,
           m_View.Height() - 12,
           m_View.Width()- 4,
           m_View.Height() - 2);

  Move(m_View.RastPort(), m_View.Width() - 90, m_View.Height() - 2 - 1);
  SetAPen(m_View.RastPort(), m_FpsPen);
  Text(m_View.RastPort(), fpsBuf, strlen(fpsBuf));
}


void PointsDisplay::UpdateInfo(const char* pInfo)
{
  if(m_View.RastPort() == NULL)
  {
    return;
  }

  SetAPen(m_View.RastPort(), m_BackPen);
  SetBPen(m_View.RastPort(), m_BackPen);
  RectFill(m_View.RastPort(),
           5,
           m_View.Height() - 12,
           90,
           m_View.Height() - 2);

  Move(m_View.RastPort(), 5, m_View.Height() - 2 - 1);
  SetAPen(m_View.RastPort(), m_InfoPen);
  Text(m_View.RastPort(), pInfo, strlen(pInfo));
}

