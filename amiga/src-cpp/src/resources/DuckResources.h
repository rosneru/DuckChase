#ifndef DUCK_RESOURCES_H
#define DUCK_RESOURCES_H

#include "AnimSeqGelsBob.h"
#include "ResourceGELs.h"

/**
 * Contains all gfx resources needed for a duck.
 *
 *
 * @author Uwe Rosner
 * @date 30/05/2020
 */
class DuckResources : public ResourceGELs
{
public:
  DuckResources();

  WORD* DefaultImage() const;
  short Width() const;
  short WordWidth() const;
  short Height() const;
  short Depth() const;

  const AnimSeqGelsBob* AnimFlyLeft() const;
  // const AnimSeqGelsBob* AnimVertically() const;


private:
  const AnimSeqGelsBob m_AnimFlyLeft;
  // const AnimSeqGelsBob m_AnimVertically;
};

#endif
