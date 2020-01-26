#include "ShapeBase.h"

ShapeBase::ShapeBase()
  : m_IsDoubleBuffered(true)
{

}

void ShapeBase::DisableDoubleBuf()
{
  m_IsDoubleBuffered = false;
}

int ShapeBase::pps2Dist(int pps, long elapsed_ms)
{
  return pps * elapsed_ms / 1000;
}
