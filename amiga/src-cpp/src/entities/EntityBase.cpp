#include <exec/types.h>

#include "EntityBase.h"
#include "ShapeBase.h"

EntityBase::EntityBase(ShapeBase* pShape) 
  : m_pShape(pShape)
{
}

int EntityBase::XPos() const
{
  if (m_pShape == NULL)
  {
    return 0;
  }

  return m_pShape->XPos();
}

int EntityBase::YPos() const
{
  if (m_pShape == NULL)
  {
    return 0;
  }
  
  return m_pShape->YPos();
}

int EntityBase::pps2Dist(int pps, long elapsed_ms)
{
  return pps * elapsed_ms / 1000;
}
