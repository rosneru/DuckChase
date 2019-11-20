#include "EntityBase.h"

int EntityBase::pps2Dist(int pps, long elapsed_ms)
{
  return pps * elapsed_ms / 1000;
}
