#include "HunterResources.h"

HunterResources::HunterResources()
  : m_AnimRunRight("AADevDuck:assets/jumpman_run_right_strip4.ilbm", 4),
    m_AnimRunLeft("AADevDuck:assets/jumpman_run_left_strip4.ilbm", 4),
    m_AnimClimb1("AADevDuck:assets/jumpman_climb1_strip2.ilbm", 2),
    m_AnimClimb2("AADevDuck:assets/jumpman_climb2_strip4.ilbm", 4)
{
  // TODO: Maybe check if all anims have same dimensions / depth and
  //       throw one exception if not.
}

WORD* HunterResources::DefaultImage() const
{
  return m_AnimRunRight[0];
}

short HunterResources::Width() const
{
  return m_AnimRunRight.Width();
}

short HunterResources::WordWidth() const
{
  return m_AnimRunRight.WordWidth();
}

short HunterResources::Height() const
{
  return m_AnimRunRight.Height();
}

short HunterResources::Depth() const
{
  return m_AnimRunRight.Depth();

}

const AnimSeqGelsBob* HunterResources::AnimRunRight() const
{
  return &m_AnimRunRight;
}

const AnimSeqGelsBob* HunterResources::AnimRunLeft() const
{
  return &m_AnimRunLeft;
}

const AnimSeqGelsBob* HunterResources::AnimRunClimb1() const
{
  return &m_AnimClimb1;
}

const AnimSeqGelsBob* HunterResources::AnimRunClimb2() const
{
  return &m_AnimClimb2;
}
