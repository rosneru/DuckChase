#ifndef RESOURCES_EXT_SPRITE_H
#define RESOURCES_EXT_SPRITE_H

#include <graphics/sprite.h>
#include "ResourceBase.h"

class ResourceExtSprite : public ResourceBase
{
public:
  virtual ExtSprite* DefaultImage() const = 0;
};

#endif
