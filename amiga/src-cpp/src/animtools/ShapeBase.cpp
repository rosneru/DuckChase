#include "ShapeBase.h"

int ShapeBase::pps2Dist(int pps, long elapsed_ms)
{
  return pps * elapsed_ms / 1000;
}
