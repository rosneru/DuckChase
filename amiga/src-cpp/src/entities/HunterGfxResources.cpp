#include "HunterGfxResources.h"

HunterGfxResources::HunterGfxResources()
  : m_AnimRightRun("AADevDuck:assets/gfx/hunter_right_run_strip2.ilbm", 2),
    m_AnimRightShoot("AADevDuck:assets/gfx/hunter_right_shoot_strip1.ilbm", 1),
    m_AnimLeftRun("AADevDuck:assets/gfx/hunter_left_run_strip2.ilbm", 2),
    m_AnimLeftShoot("AADevDuck:assets/gfx/hunter_left_shoot_strip1.ilbm", 1)
{
  // TODO: Maybe check if all anims have same dimensions / depth and
  //       throw an exception if not.
}

WORD* HunterGfxResources::DefaultImage() const
{
  return m_AnimRightRun[0];
}

short HunterGfxResources::Width() const
{
  return m_AnimRightRun.Width();
}

short HunterGfxResources::WordWidth() const
{
  return m_AnimRightRun.WordWidth();
}

short HunterGfxResources::Height() const
{
  return m_AnimRightRun.Height();
}

short HunterGfxResources::Depth() const
{
  return m_AnimRightRun.Depth();
}

const AnimSeqGelsBob* HunterGfxResources::AnimRightRun() const
{
  return &m_AnimRightRun;
}

const AnimSeqGelsBob* HunterGfxResources::AnimLeftRun() const
{
  return &m_AnimLeftRun;
}

const AnimSeqGelsBob* HunterGfxResources::AnimRightShoot() const
{
  return &m_AnimRightShoot;
}

const AnimSeqGelsBob* HunterGfxResources::AnimLeftShoot() const
{
  return &m_AnimLeftShoot;
}
