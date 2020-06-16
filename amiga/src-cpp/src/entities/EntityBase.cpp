#include "EntityBase.h"

EntityBase::EntityBase(const GameWorld& gameWorld, 
                       ShapeBase& shape,
                       const ShadowMask*& currentMask) 
  : m_GameWorld(gameWorld),
    m_Shape(shape),
    m_pCurrentShadowMask(currentMask),
    m_XSpeed(0),
    m_YSpeed(0),
    m_IsAlive(true),
    m_FrameSwitchingRateAt50Fps(8)
{
}

ShapeBase& EntityBase::Shape() const
{
  return m_Shape;
}


long EntityBase::XSpeed() const
{
  return m_XSpeed;
}


long EntityBase::YSpeed() const
{
  return m_YSpeed;
}


bool EntityBase::IsAlive() const
{
  return m_IsAlive;
}

bool EntityBase::isCollision(EntityBase& other) const
{
  if(m_Shape.Intersects(other.Shape()))
  {
    // Also calculate the other shapes intersection/collsion rect
    other.Shape().Intersects(m_Shape);

    if(m_pCurrentShadowMask->IsCollision(other.m_pCurrentShadowMask,
                                         m_Shape.IntersectRect(),
                                         other.Shape().IntersectRect()))
    {
      return true;
    }

  }

  return false;
}


long EntityBase::pps2Dist(long pps, long elapsed_ms)
{
  bool isNeg = pps < 0;

  if(elapsed_ms == 0)
  {
    // Defaults to '25 fps' if
    elapsed_ms = 40;
  }

  if(pps == 0)
  {
    return 0;
  }

  if(isNeg)
  {
    pps = -pps;
  }

  // This formula basically is 'pps * elapsed_ms / 1000' but with
  // enhancement:
  //   - division by 1000 is replaced by right shift >> 10, which in
  //     result is a division by 1024
  //   - Added a correction factor of 2 to minimize errors 
  long dist = (pps * (elapsed_ms + 2)) >> 10;
  if(dist == 0)
  {
    // Return minimum value if calculation result is 0
    if(isNeg)
    {
      return -1;
    }
    else
    {
      return 1;
    }
    
  }

  if(isNeg)
  {
    return -dist;
  }
  else
  {
    return dist;
  }
}
