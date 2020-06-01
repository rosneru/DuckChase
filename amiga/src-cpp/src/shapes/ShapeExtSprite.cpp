#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

#include <graphics/gels.h>

#include "ShapeExtSprite.h"

#include <stdio.h>

ShapeExtSprite::ShapeExtSprite(struct ViewPort* pViewPort,
                               const ResourceExtSprite& gfxResources,
                               bool stealMouse)
  : ShapeBase(gfxResources),
    m_pViewPort(pViewPort),
    m_pEmptySprite(NULL),
    m_pCurrentSprite(NULL),
    m_SpriteNumberGot(-1),
    m_SpriteNumber(-1)
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
  m_pCurrentSprite = gfxResources.DefaultImage();
  m_SpriteNumberGot = GetExtSprite(m_pCurrentSprite, TAG_DONE);

  if (m_SpriteNumberGot < 0)
  {
    throw "ShapeVSprite: No hardware sprite available.";
  }

  if(stealMouse == false)
  {
    m_SpriteNumber = m_SpriteNumberGot;
    return;
  }

  // AABoing: Relatively safe way to replace the mouse pointer (sprite
  // 0) with the arrow sprite
  m_SpriteNumber = 0;
  m_pCurrentSprite->es_SimpleSprite.num = m_SpriteNumber;
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
  struct ExtSprite* pOldSprite = m_pCurrentSprite;
  m_pCurrentSprite = pNewImage;
  ChangeExtSprite(m_pViewPort, pOldSprite, m_pCurrentSprite, TAG_DONE);
}


void ShapeExtSprite::SetVPortColorsForSprite(struct ViewPort* pViewPort,
                                             ULONG* pColors)
{
  if(pColors == NULL)
  {
    return;
  }

  // Which pens to set depends on the used sprite number
  size_t startPen = 16 + ((m_SpriteNumber & 0x06) << 1);

  // 'Or' the start pen into the first item of color table
  *pColors |= startPen;

  LoadRGB32(pViewPort, pColors);
}


int ShapeExtSprite::Left() const
{
  if (m_pCurrentSprite == NULL)
  {
    return 0;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*)m_pCurrentSprite;
  return pSpr->x;
}


int ShapeExtSprite::Top() const
{
  if (m_pCurrentSprite == NULL)
  {
    return 0;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*)m_pCurrentSprite;
  return pSpr->y;
}

int ShapeExtSprite::Right() const
{
  return ((struct SimpleSprite*)m_pCurrentSprite)->x + m_Width;
}

int ShapeExtSprite::Bottom() const
{
  return ((struct SimpleSprite*)m_pCurrentSprite)->x + m_Height;
}

void ShapeExtSprite::SetInvisible()
{
  if ((m_pCurrentSprite == NULL) || (m_pViewPort == NULL))
  {
    return;
  }

  ChangeExtSprite(m_pViewPort, m_pCurrentSprite, m_pEmptySprite, TAG_END);
}


void ShapeExtSprite::SetVisible()
{
  if ((m_pCurrentSprite == NULL) ||
      (m_pViewPort == NULL))
  {
    return;
  }

  ChangeExtSprite(m_pViewPort, m_pEmptySprite, m_pCurrentSprite, TAG_END);
}


bool ShapeExtSprite::IsVisible() const
{
  if ((m_pCurrentSprite == NULL) ||
      (m_pViewPort == NULL))
  {
    return false;
  }

  return m_pCurrentSprite != m_pEmptySprite;
}


bool ShapeExtSprite::IsGone() const
{
  if ((m_pCurrentSprite == NULL) ||
      (m_pViewPort == NULL))
  {
    return true;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*)m_pCurrentSprite;

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
  if ((m_pCurrentSprite == NULL) || (m_pViewPort == NULL))
  {
    return;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*)m_pCurrentSprite;
  MoveSprite(m_pViewPort, pSpr, x, y);
}