#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>

#include <graphics/gels.h>

#include "ShapeSprite.h"
#include "animtools_proto.h"

ShapeSprite::ShapeSprite()
  : m_pAnimSeq(NULL),
    m_pViewPort(NULL),
    m_pEmptySprite(NULL),
    m_pCurrentSprite(NULL),
    m_SpriteNumberGot(-1),
    m_SpriteNumberUsing(-1)
{
}

ShapeSprite::~ShapeSprite()
{
  if (m_pEmptySprite != NULL)
  {
    FreeSpriteData(m_pEmptySprite);
    m_pEmptySprite = NULL;
  }

  if (m_SpriteNumberGot >= 0)
  {
    FreeSprite(m_SpriteNumberGot);
    m_SpriteNumberGot = -1;
  }
}

void ShapeSprite::SetVPortColorsForSprite(struct ViewPort* pViewPort,
                                          ULONG* pColors)
{
  // Which 3 pens to set depends on the used sprite number
  size_t startPen = 16 + ((m_SpriteNumberUsing & 0x06) << 1);

  // But the first of the 4 sprite pens is always unused
  startPen++;

  size_t numCols = 3;
  size_t iColArray = 0;
  for(size_t iPen = startPen; iPen < (startPen + numCols); iPen++)
  {
    ULONG r = pColors[iColArray++];
    ULONG g = pColors[iColArray++];
    ULONG b = pColors[iColArray++];
    SetRGB32(pViewPort, iPen, r, g, b);
  }
}

void ShapeSprite::UseMouseSprite()
{
  if(m_pCurrentSprite == NULL)
  {
    return;
  }

  if(m_SpriteNumberUsing == 0)
  {
    // Already done
    return;
  }

  // This sprite should replace the mouse pointer. So we set ist sprite
  // number to 0, which is the mouse pointers sprite number. See AABoing
  // source from Aminet

  m_SpriteNumberUsing = 0;
  m_pCurrentSprite->es_SimpleSprite.num = m_SpriteNumberUsing;
}

void ShapeSprite::SetViewPort(struct ViewPort* pViewPort)
{
  if (m_pViewPort != NULL)
  {
    // Already set
    return;
  }

  m_pViewPort = pViewPort;
}

int ShapeSprite::XPos() const
{
  if (m_pCurrentSprite == NULL)
  {
    return 0;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*)m_pCurrentSprite;
  return pSpr->x;
}

int ShapeSprite::YPos() const
{
  if (m_pCurrentSprite == NULL)
  {
    return 0;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*)m_pCurrentSprite;
  return pSpr->y;
}

int ShapeSprite::Width() const
{
  return m_pAnimSeq->GetWidth();
}

int ShapeSprite::Height() const
{
  return m_pAnimSeq->GetHeight();
}

void ShapeSprite::Move(int x, int y)
{
  if ((m_pCurrentSprite == NULL) || (m_pViewPort == NULL))
  {
    return;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*)m_pCurrentSprite;
  MoveSprite(m_pViewPort, pSpr, x, y);
}

void ShapeSprite::SetInvisible()
{
  if ((m_pCurrentSprite == NULL) || (m_pViewPort == NULL))
  {
    return;
  }

  struct ExtSprite* pOldSprite = m_pCurrentSprite;
  m_pCurrentSprite = m_pEmptySprite;
  ChangeExtSprite(m_pViewPort, pOldSprite, m_pCurrentSprite, TAG_END);
}

void ShapeSprite::SetVisible()
{
  if ((m_pCurrentSprite == NULL) ||
      (m_pViewPort == NULL) ||
      (m_pAnimSeq == NULL))
  {
    return;
  }

  struct ExtSprite* pOldSprite = m_pCurrentSprite;
  m_pCurrentSprite = m_pAnimSeq->GetCurrentImage();
  ChangeExtSprite(m_pViewPort, pOldSprite, m_pCurrentSprite, TAG_END);
}

bool ShapeSprite::IsVisible() const
{
  if ((m_pCurrentSprite == NULL) ||
      (m_pViewPort == NULL))
  {
    return false;
  }

  return m_pCurrentSprite != m_pEmptySprite;
}

void ShapeSprite::SetAnimSequence(AnimSeqBase* pAnimSequence)
{
  if(pAnimSequence == NULL)
  {
    return;
  }

  m_pAnimSeq = static_cast<AnimSeqSprite*>(pAnimSequence);
  if(m_SpriteNumberGot < 0)
  {
    // If no sprite object exists, create it
    createSprite();
  }
}

void ShapeSprite::NextImage()
{
  if (m_pCurrentSprite == m_pEmptySprite)
  {
    // Only animate when sprite is visible
    return;
  }

  if ((m_pCurrentSprite == NULL) ||
      (m_pViewPort == NULL) ||
      (m_pAnimSeq == NULL))
  {
    return;
  }


  struct ExtSprite* pNextImg = m_pAnimSeq->GetNextImage();
  if (pNextImg == NULL)
  {
    return;
  }

  struct ExtSprite* pOldSprite = m_pCurrentSprite;
  m_pCurrentSprite = pNextImg;
  ChangeExtSprite(m_pViewPort, pOldSprite, m_pCurrentSprite, TAG_DONE);
}

void ShapeSprite::createSprite()
{
  if ((m_pAnimSeq == NULL) || (m_SpriteNumberGot >= 0))
  {
    return;
  }

  // Allocate the empty sprite data (for setting sprite invisible)
  if (m_pEmptySprite == NULL)
  {
    struct BitMap* pBitMap = AllocBitMap(m_pAnimSeq->GetWidth(),
                                         m_pAnimSeq->GetHeight(),
                                         m_pAnimSeq->GetDepth(),
                                         BMF_CLEAR,
                                         NULL);
    if(pBitMap == NULL)
    {
      // Not enough memory for empty sprite bitmap
      return;
    }

    m_pEmptySprite = AllocSpriteData(pBitMap, TAG_END);
    if(m_pEmptySprite == NULL)
    {
      // Not enough memory for empty sprite
      return;
    }

    FreeBitMap(pBitMap);
  }

  // Trying to allocate a hardware sprite
  m_pCurrentSprite = m_pAnimSeq->GetFirstImage();
  m_SpriteNumberGot = GetExtSprite(m_pCurrentSprite, TAG_DONE);

  if (m_SpriteNumberGot < 0)
  {
    // No hardware sprite available
    m_pCurrentSprite = NULL;
  }
}
