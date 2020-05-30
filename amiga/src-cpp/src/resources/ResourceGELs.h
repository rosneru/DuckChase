#ifndef RESOURCES_GELS_H
#define RESOURCES_GELS_H

#include <exec/types.h>
#include "ResourceBase.h"

class ResourceGELs : public ResourceBase
{
public:
  virtual WORD* DefaultImage() const = 0;
};

#endif
