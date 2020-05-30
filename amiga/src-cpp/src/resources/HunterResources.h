#ifndef JUMPMAN_RESOURCES_H
#define JUMPMAN_RESOURCES_H

#include "AnimSeqGelsBob.h"
#include "ResourceGELs.h"

/**
 * Contains all gfx resources needed for a hunter.
 *
 *
 * @author Uwe Rosner
 * @date 30/05/2020
 */
class HunterResources : public ResourceGELs
{
public:
  HunterResources();

  WORD* DefaultImage() const;
  short Width() const;
  short WordWidth() const;
  short Height() const;
  short Depth() const;

  const AnimSeqGelsBob* AnimRunRight() const;
  const AnimSeqGelsBob* AnimRunLeft() const;
  const AnimSeqGelsBob* AnimRunClimb1() const;
  const AnimSeqGelsBob* AnimRunClimb2() const;

private:
  const AnimSeqGelsBob m_AnimRunRight;
  const AnimSeqGelsBob m_AnimRunLeft;
  const AnimSeqGelsBob m_AnimClimb1;
  const AnimSeqGelsBob m_AnimClimb2;
};

#endif
