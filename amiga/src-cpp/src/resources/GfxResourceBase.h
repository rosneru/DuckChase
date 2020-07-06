#ifndef RESOURCE_BASE_H
#define RESOURCE_BASE_H

/**
 * Base class for resources as anims and sound of the entities.
 *
 *
 * @author Uwe Rosner
 * @date 26/05/2020
 */
class GfxResourceBase
{
public:
  virtual short Width() const = 0;
  virtual short WordWidth() const = 0;
  virtual short Height() const = 0;
  virtual short Depth() const = 0;
};

#endif
