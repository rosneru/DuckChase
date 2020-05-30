#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <graphics/gfxbase.h>

#include <stdio.h>
#include <string.h>

#include "InfoDisplay.h"

extern struct GfxBase* GfxBase;


InfoDisplay::InfoDisplay(GameViewBase& gameView, 
                         GameColors& gameColors,
                         const size_t& maxArrows, 
                         const size_t& maxStrain)
  : m_View(gameView),
    m_MaxArrows(maxArrows),
    m_MaxStrain(maxStrain),
    m_FormerStrain(0),
    m_StrainSpreadColors(gameColors.GetStrainSpreadColors()),
    //m_ArrowDisabledImg("gfx/arrow_palette2.raw", 16, 8, 4), 
    //m_ArrowEnabledImg("gfx/arrow_palette3.raw", 16, 8, 4),
    m_Right(m_View.Width() - 4),
    m_Bottom(m_View.Height() - 1)
{

}


InfoDisplay::~InfoDisplay()
{

}


// void InfoDisplay::UpdateArrows(size_t arrowsLeft)
// {
// //  if((m_ArrowDisabledImg.GetBitMap() == NULL)
// //   ||(m_ArrowEnabledImg.GetBitMap() == NULL))
// //  {
// //    return;
// //  }

  // // m_NumArrowsLeft = numArrowsLeft; // TODO Should be done in game!!!

  // // Start blitting with an highlighted image for the available arrows
  // struct BitMap* pBitMap = m_ArrowEnabledImg.GetBitMap();

  // for(size_t i = 0; i < m_MaxArrows; i++)
  // {
    // if(i >= arrowsLeft)
    // {
      // // Blit the already shot arrows with a non-highlighted image
      // pBitMap = m_ArrowDisabledImg.GetBitMap();
    // }

    // BltBitMapRastPort(pBitMap, 
                      // 0, 
                      // 0, 
                      // m_View.RastPort(),
                      // 110 + i * (m_ArrowEnabledImg.Width() + 5),
                      // m_Bottom - m_ArrowEnabledImg.Height() - 1,
                      // m_ArrowEnabledImg.Width(),
                      // m_ArrowEnabledImg.Height(),
                      // 0xC0);


  // }
// }


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
           m_Right - 16, 
           m_Bottom - 11,
           m_Right, 
           m_Bottom); // TODO check 'm_Bottom - 1'

  SetAPen(m_View.RastPort(), frontPen);
  Move(m_View.RastPort(), m_Right - 22, m_Bottom - 3);
  Text(m_View.RastPort(), fpsBuf, strlen(fpsBuf));
}


void InfoDisplay::UpdateStrain(size_t newStrain)
{
  if(newStrain > m_MaxStrain)
  {
    newStrain = m_MaxStrain;
  }

  if(newStrain == 0)
  {
    // Clear the display
    SetAPen(m_View.RastPort(), 0);
    EraseRect(m_View.RastPort(), 401, 247, 401 + 117, 247 + 5);

    m_FormerStrain = 0;
    return;
  }

  ULONG m_StrainColor[] =
  {
    0x0001000F, // 0x0001 - Load 1 color, starting from 0x00F
    0x00000000, 0x00000000, 0x00000000,
    0x00000000,
  };

  if(newStrain %2 == 0)
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
           401 + m_FormerStrain,
           247,
           400 + newStrain,
           247 + 5);

  m_FormerStrain = newStrain;
}
