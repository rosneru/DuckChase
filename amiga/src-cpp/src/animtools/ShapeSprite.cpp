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
    m_HardwareSpriteNumber(-1)
{
}

ShapeSprite::~ShapeSprite()
{
  if (m_pEmptySprite != NULL)
  {
    FreeSpriteData(m_pEmptySprite);
    m_pEmptySprite = NULL;
  }

  if (m_HardwareSpriteNumber >= 0)
  {
    FreeSprite(m_HardwareSpriteNumber);
    m_HardwareSpriteNumber = -1;
  }
}

struct ExtSprite* ShapeSprite::Get()
{
  return m_pCurrentSprite;
}

int ShapeSprite::SpriteNumber()
{
  return m_HardwareSpriteNumber;
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
  if(m_HardwareSpriteNumber < 0)
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
  if ((m_pAnimSeq == NULL) || (m_HardwareSpriteNumber >= 0))
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
  m_HardwareSpriteNumber = GetExtSprite(m_pCurrentSprite, TAG_DONE);

  if (m_HardwareSpriteNumber < 0)
  {
    // No hardware sprite available
    m_pCurrentSprite = NULL;
  }
}
