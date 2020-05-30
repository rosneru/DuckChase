#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <graphics/gels.h>

#include "ShapeVSprite.h"

ShapeVSprite::ShapeVSprite(struct RastPort* pRastPort, 
                           const ResourceGELs& gfxResources,
                           WORD* pSpriteColors)
  : ShapeBase(gfxResources),
    m_pRastPort(pRastPort),
    m_pSpriteColors(pSpriteColors),
    m_pSprite(NULL),
    m_bIsVisible(false),
    m_LineSize(sizeof(WORD) * m_WordWidth),
    m_PlaneSize(m_LineSize * m_Height)
{
    m_Width = 16;
    m_WordWidth = 1;
    m_Height = gfxResources.Height();
    m_Depth = 2;

  m_pSprite = (struct VSprite*) AllocVec(sizeof(struct VSprite), MEMF_CLEAR);
  if(m_pSprite == NULL)
  {
    throw "ShapeVSprite: Failed to allocate memory for vsprite.";
  }

  m_pSprite->BorderLine = (WORD*) AllocVec(m_LineSize, MEMF_CHIP);
  if(m_pSprite->BorderLine == NULL)
  {
    throw "ShapeVSprite: Failed to allocate memory for border line.";
  }

  m_pSprite->CollMask = (WORD*) AllocVec(m_PlaneSize, MEMF_CHIP);
  if(m_pSprite->CollMask == NULL)
  {
    throw "ShapeVSprite: Failed to allocate memory for collision mask.";
  }

  m_pSprite->X = 0;
  m_pSprite->Y = 0;
  m_pSprite->Flags = VSPRITE;
  m_pSprite->Width = m_WordWidth;
  m_pSprite->Depth = m_Depth;
  m_pSprite->Height = m_Height;
  m_pSprite->MeMask = 0;
  m_pSprite->HitMask = 0;
  m_pSprite->ImageData = gfxResources.DefaultImage();
  m_pSprite->SprColors = m_pSpriteColors;
  m_pSprite->PlanePick  = 0x0; // Will be set to proper values after VSprite creation
  m_pSprite->PlaneOnOff = 0x0;
  InitMasks(m_pSprite);


  AddVSprite(m_pSprite, m_pRastPort);
  m_bIsVisible = true;
}

ShapeVSprite::~ShapeVSprite()
{
  SetInvisible();

  if(m_pSprite != NULL)
  {
    if(m_pSprite->BorderLine != NULL)
    {
      FreeVec(m_pSprite->BorderLine);
      m_pSprite->BorderLine = NULL;
    }

    if(m_pSprite->CollMask != NULL)
    {
      FreeVec(m_pSprite->CollMask);
      m_pSprite->CollMask = NULL;
    }

    FreeVec(m_pSprite);
    m_pSprite = NULL;
  }
}



int ShapeVSprite::Left() const
{
  if (m_pSprite == NULL)
  {
    return 0;
  }

  return m_pSprite->X;
}

int ShapeVSprite::Top() const
{
  if (m_pSprite == NULL)
  {
    return 0;
  }

  return m_pSprite->Y;
}


void ShapeVSprite::move(int x, int y)
{
  if (m_pSprite == NULL)
  {
    return;
  }

  m_pSprite->X = x;
  m_pSprite->Y = y;
}

void ShapeVSprite::SetInvisible()
{
  if (m_bIsVisible == false)
  {
    // Already invisible
    return;
  }

  if (m_pSprite == NULL)
  {
    return;
  }

  RemVSprite(m_pSprite);
  m_bIsVisible = false;
}

void ShapeVSprite::SetVisible()
{
  if (m_bIsVisible == true)
  {
    // Already visible
    return;
  }

  if (m_pSprite == NULL)
  {
    return;
  }

  if (m_pRastPort == NULL)
  {
    return;
  }

  AddVSprite(m_pSprite, m_pRastPort);
  m_bIsVisible = true;
}

bool ShapeVSprite::IsVisible() const
{
  return m_bIsVisible;
}

bool ShapeVSprite::IsGone() const
{
  if(m_pSprite == NULL)
  {
    return false;
  }

  return (m_pSprite->Flags & GELGONE) != 0;
}

void ShapeVSprite::SetImage(WORD* pImage)
{
  m_pSprite->ImageData = pImage;
  InitMasks(m_pSprite);
}
