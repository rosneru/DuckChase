#include <exec/types.h>

#include "EntityBase.h"
#include "ShapeBase.h"

EntityBase::EntityBase(ShapeBase* pShape) 
  : m_pShape(pShape)
{
}

void EntityBase::DisableDoubleBuf()
{
  if (m_pShape == NULL)
  {
    return;
  }

  m_pShape->DisableDoubleBuf();
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
  if(elapsed_ms == 0)
  {
    elapsed_ms = 40;
  }

  return pps / (1000 / elapsed_ms);
}
