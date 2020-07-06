#ifndef RESOURCES_GELS_H
#define RESOURCES_GELS_H

#include <exec/types.h>
#include "GfxResourceBase.h"

/**
 * GELS resources must provide a DefaultImage as WORD* ('ImageData').
 */
class GfxResourceGELs : public GfxResourceBase
{
public:
  virtual WORD* DefaultImage() const = 0;
};

#endif
