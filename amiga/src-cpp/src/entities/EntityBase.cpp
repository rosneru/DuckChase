#include "EntityBase.h"

EntityBase::EntityBase(const GameWorld& gameWorld) 
  : m_GameWorld(gameWorld),
    m_XSpeed_pps(0),
    m_YSpeed_pps(0),
    m_bIsAlive(true),
    m_FrameSwitchingRateAt50Fps(8)
{
}

long EntityBase::XSpeed_pps()
{
  return m_XSpeed_pps;
}


long EntityBase::YSpeed_pps()
{
  return m_YSpeed_pps;
}


bool EntityBase::IsAlive()
{
  return m_bIsAlive;
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
