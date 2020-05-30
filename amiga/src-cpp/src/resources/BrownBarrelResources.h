#ifndef BROWN_BARREL_RESOURCES_H
#define BROWN_BARREL_RESOURCES_H

#include "AnimSeqGelsBob.h"
#include "ResourceGELs.h"

/**
 * Contains all gfx resources needed for a BrownBarrel.
 *
 *
 * @author Uwe Rosner
 * @date 26/05/2020
 */
class BrownBarrelResources : public ResourceGELs
{
public:
  BrownBarrelResources();

  WORD* DefaultImage() const;
  short Width() const;
  short WordWidth() const;
  short Height() const;
  short Depth() const;

  const AnimSeqGelsBob* AnimHorizontally() const;
  // const AnimSeqGelsBob* AnimVertically() const;


private:
  const AnimSeqGelsBob m_AnimHorizontally;
  // const AnimSeqGelsBob m_AnimVertically;
};

#endif
