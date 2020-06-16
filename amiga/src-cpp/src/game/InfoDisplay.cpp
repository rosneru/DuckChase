#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <graphics/gfxbase.h>

#include <stdio.h>
#include <string.h>

#include "InfoDisplay.h"

extern struct GfxBase* GfxBase;


InfoDisplay::InfoDisplay(GameViewBase& gameView, 
                         const GameColors& gameColors,
                         GameVars& gameVars)
  : m_View(gameView),
    m_GameVars(gameVars),
    m_BackgroundPicture("AADevDuck:assets/info_bar.ilbm", false, false),
    m_ArrowsRect(110, gameView.Height() - 29),
    m_FpsRect(gameView.Width() - 30, gameView.Height() - 18),
    m_StrainRect(322, 227),
    m_FormerStrain(0),
    m_StrainSpreadColors(gameColors.GetStrainSpreadColors()),
    m_ArrowImages("AADevDuck:assets/arrow_shadow_strip2.ilbm", 2)
{
  m_ArrowsRect.SetWidthHeight(94, 7);
  m_FpsRect.SetWidthHeight(19, 7);
  m_StrainRect.SetWidthHeight(m_GameVars.MaxStrain(), 7);

  m_View.Render();

  // Blit the InfoDisplay background picture to tzhe bottom
  m_View.BlitPicture(m_BackgroundPicture, 
                     0,
                     m_View.Height() - m_BackgroundPicture.Height());
}


InfoDisplay::~InfoDisplay()
{

}


void InfoDisplay::UpdateArrows()
{
  // Start blitting with an highlighted image for the available arrows
  struct BitMap* pBitMap = m_ArrowImages[0];

  for(size_t i = 0; i < m_GameVars.MaxArrows(); i++)
  {
    if(i >= m_GameVars.NumArrows())
    {
      // Blit the already shot arrows with a non-highlighted image
      pBitMap = m_ArrowImages[1];
    }

    BltBitMapRastPort(pBitMap, 
                      0, 
                      0, 
                      m_View.RastPort(),
                      m_ArrowsRect.Left() + i * (m_ArrowImages.Width() + 5),
                      m_ArrowsRect.Top(),
                      m_ArrowImages.Width(),
                      m_ArrowImages.Height(),
                      0xC0);
  }
}


void InfoDisplay::UpdateFps(size_t fps)
{
  short backPen = 0;
  short frontPen = 6;

  char fpsBuf[16];
  sprintf(fpsBuf, "%d", fps);

  // Drawing a filled black rect at the bottom of the view
  SetAPen(m_View.RastPort(), backPen);
  SetBPen(m_View.RastPort(), backPen);
  RectFill(m_View.RastPort(),
           m_FpsRect.Left(), 
           m_FpsRect.Top(),
           m_FpsRect.Right(), 
           m_FpsRect.Bottom());

  SetAPen(m_View.RastPort(), frontPen);
  Move(m_View.RastPort(), m_FpsRect.Left(), m_FpsRect.Top() + 8); // + 8 .. font baseline height. TODO: remove constant.
  Text(m_View.RastPort(), fpsBuf, strlen(fpsBuf));
}


void InfoDisplay::UpdateStrain(size_t newStrain, bool isForSecondBuffer)
{
  if(newStrain > m_GameVars.MaxStrain())
  {
    newStrain = m_GameVars.MaxStrain();
  }

  if(newStrain == 0)
  {
    // Clear the display
    SetAPen(m_View.RastPort(), 0);
    EraseRect(m_View.RastPort(), 
              m_StrainRect.Left(),
              m_StrainRect.Top(), 
              m_StrainRect.Right(),
              m_StrainRect.Bottom());

    m_FormerStrain = 0;
    return;
  }

  ULONG m_StrainColor[] =
  {
    0x0001000F, // 0x0001 - Load 1 color, starting from 0x00F
    0x00000000, 0x00000000, 0x00000000,
    0x00000000,
  };

  if(newStrain % 2 == 0)
  {
    int idx = 3 * newStrain / 2;
    m_StrainColor[1] = m_StrainSpreadColors[idx];
    m_StrainColor[2] = m_StrainSpreadColors[idx + 1];
    m_StrainColor[3] = m_StrainSpreadColors[idx + 2];
    LoadRGB32(m_View.ViewPort(), m_StrainColor);
  }

  // TODO remove constants
  SetAPen(m_View.RastPort(), 15);
  RectFill(m_View.RastPort(),
           m_StrainRect.Left() + 1 + m_FormerStrain,
           m_StrainRect.Top(),
           m_StrainRect.Left() + newStrain,
           m_StrainRect.Bottom());

  if(isForSecondBuffer)
  {
    m_FormerStrain = newStrain;
  }

}
