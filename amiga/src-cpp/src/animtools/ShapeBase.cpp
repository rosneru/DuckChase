#include "ShapeBase.h"

ShapeBase::ShapeBase()
  : m_IsDoubleBuffered(true)
{

}

void ShapeBase::DisableDoubleBuf()
{
  m_IsDoubleBuffered = false;
}
