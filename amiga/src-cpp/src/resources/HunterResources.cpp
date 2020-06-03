#include "HunterResources.h"

HunterResources::HunterResources()
  : m_AnimRightRun("AADevDuck:assets/hunter_right_run_strip2.iff", 2),
    m_AnimRightShoot("AADevDuck:assets/hunter_right_shoot_strip1.iff", 1),
    m_AnimLeftRun("AADevDuck:assets/hunter_left_run_strip2.iff", 2),
    m_AnimLeftShoot("AADevDuck:assets/hunter_left_shoot_strip1.iff", 1)
{
  // TODO: Maybe check if all anims have same dimensions / depth and
  //       throw an exception if not.
}

WORD* HunterResources::DefaultImage() const
{
  return m_AnimRightRun[0];
}

short HunterResources::Width() const
{
  return m_AnimRightRun.Width();
}

short HunterResources::WordWidth() const
{
  return m_AnimRightRun.WordWidth();
}

short HunterResources::Height() const
{
  return m_AnimRightRun.Height();
}

short HunterResources::Depth() const
{
  return m_AnimRightRun.Depth();
}

const AnimSeqGelsBob* HunterResources::AnimRightRun() const
{
  return &m_AnimRightRun;
}

const AnimSeqGelsBob* HunterResources::AnimLeftRun() const
{
  return &m_AnimLeftRun;
}

const AnimSeqGelsBob* HunterResources::AnimRightShoot() const
{
  return &m_AnimRightShoot;
}

const AnimSeqGelsBob* HunterResources::AnimLeftShoot() const
{
  return &m_AnimLeftShoot;
}
