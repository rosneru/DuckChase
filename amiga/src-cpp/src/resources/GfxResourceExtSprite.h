#ifndef RESOURCES_EXT_SPRITE_H
#define RESOURCES_EXT_SPRITE_H

#include <graphics/sprite.h>
#include "GfxResourceBase.h"

/**
 * ExtSprite resources must provide a DefaultImage in ExtSprite format.
 */
class GfxResourceExtSprite : public GfxResourceBase
{
public:
  virtual ExtSprite* DefaultImage() const = 0;
};

#endif
