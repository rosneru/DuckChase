#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

#include <graphics/gels.h>

#include "ShapeExtSprite.h"

#include <stdio.h>

ShapeExtSprite::ShapeExtSprite(struct ViewPort* pViewPort,
                               const GfxResourceExtSprite& gfxResources)
  : ShapeBase(gfxResources.Width(), 
              gfxResources.Height(), 
              gfxResources.Depth()),
    m_pViewPort(pViewPort),
    m_pEmptySprite(NULL),
    m_pSprite(NULL),
    m_SpriteNumberGot(-1)
{
  // Allocate the empty sprite data (for setting sprite invisible)
  struct BitMap* pEmptyBitMap = AllocBitMap(m_Width,
                                            m_Height,
                                            m_Depth,
                                            BMF_CLEAR,
                                            NULL);
  if(pEmptyBitMap == NULL)
  {
    throw "ShapeVSprite: Failed to allocate empty sprite bitmap.";
  }

  m_pEmptySprite = AllocSpriteData(pEmptyBitMap, TAG_END);
  FreeBitMap(pEmptyBitMap);
  if(m_pEmptySprite == NULL)
  {
    throw "ShapeVSprite: Failed to allocate empty sprite data.";
  }

  // Trying to get a hardware sprite
  m_pSprite = gfxResources.DefaultImage();
  m_SpriteNumberGot = GetExtSprite(m_pSprite, TAG_DONE);

  if (m_SpriteNumberGot < 0)
  {
    throw "ShapeVSprite: No hardware sprite available.";
  }
}


ShapeExtSprite::~ShapeExtSprite()
{
  if (m_SpriteNumberGot >= 0)
  {
    FreeSprite(m_SpriteNumberGot);
    m_SpriteNumberGot = -1;
  }


  if (m_pEmptySprite != NULL)
  {
    FreeSpriteData(m_pEmptySprite);
    m_pEmptySprite = NULL;
  }

}


void ShapeExtSprite::SetImage(ExtSprite* pNewImage)
{
  struct ExtSprite* pOldSprite = m_pSprite;
  m_pSprite = pNewImage;
  ChangeExtSprite(m_pViewPort, pOldSprite, m_pSprite, TAG_DONE);
}


void ShapeExtSprite::SetVPortColorsForSprite(struct ViewPort* pViewPort,
                                             ULONG* pColors)
{
  if(pColors == NULL)
  {
    return;
  }

  // Which pens to set depends on the used sprite number
  size_t startPen = 16 + ((m_SpriteNumberGot & 0x06) << 1);

  // 'Or' the start pen into the first item of color table
  *pColors |= startPen;

  LoadRGB32(pViewPort, pColors);
}


int ShapeExtSprite::Left() const
{
  if (m_pSprite == NULL)
  {
    return 0;
  }

  return m_pSprite->es_SimpleSprite.x;
}


int ShapeExtSprite::Top() const
{
  if (m_pSprite == NULL)
  {
    return 0;
  }

  return m_pSprite->es_SimpleSprite.y;
}

int ShapeExtSprite::Right() const
{
  // right = width - 1 + left
  return m_Width - 1 + m_pSprite->es_SimpleSprite.x;
}

int ShapeExtSprite::Bottom() const
{
  // bottom = height - 1 + top
  return m_Height - 1 + m_pSprite->es_SimpleSprite.y;
}

void ShapeExtSprite::SetInvisible()
{
  if ((m_pSprite == NULL) || (m_pViewPort == NULL))
  {
    return;
  }

  ChangeExtSprite(m_pViewPort, m_pSprite, m_pEmptySprite, TAG_END);
}


void ShapeExtSprite::SetVisible()
{
  if ((m_pSprite == NULL) ||
      (m_pViewPort == NULL))
  {
    return;
  }

  ChangeExtSprite(m_pViewPort, m_pEmptySprite, m_pSprite, TAG_END);
}


bool ShapeExtSprite::IsVisible() const
{
  if ((m_pSprite == NULL) ||
      (m_pViewPort == NULL))
  {
    return false;
  }

  return m_pSprite != m_pEmptySprite;
}


bool ShapeExtSprite::IsGone() const
{
  if ((m_pSprite == NULL) ||
      (m_pViewPort == NULL))
  {
    return true;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*)m_pSprite;

  if(pSpr->x < - Width() || pSpr->x > m_pViewPort->DWidth)
  {
    return true;
  }

  if(pSpr->y < - Height() || pSpr->y > m_pViewPort->DHeight)
  {
    return true;
  }

  return false;
}


void ShapeExtSprite::move(int x, int y)
{
  if ((m_pSprite == NULL) || (m_pViewPort == NULL))
  {
    return;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*)m_pSprite;
  MoveSprite(m_pViewPort, pSpr, x, y);
}
