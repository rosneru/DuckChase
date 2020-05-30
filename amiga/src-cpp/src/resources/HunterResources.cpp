#include "HunterResources.h"

JumpmanResources::JumpmanResources()
  : m_AnimRunRight("AADevDuck:assets/jumpman_run_right_strip4.ilbm", 4),
    m_AnimRunLeft("AADevDuck:assets/jumpman_run_left_strip4.ilbm", 4),
    m_AnimClimb1("AADevDuck:assets/jumpman_climb1_strip2.ilbm", 2),
    m_AnimClimb2("AADevDuck:assets/jumpman_climb2_strip4.ilbm", 4)
{
  // TODO: Maybe check if all anims have same dimensions / depth and
  //       throw one exception if not.
}

WORD* JumpmanResources::DefaultImage() const
{
  return m_AnimRunRight[0];
}

short JumpmanResources::Width() const
{
  return m_AnimRunRight.Width();
}

short JumpmanResources::WordWidth() const
{
  return m_AnimRunRight.WordWidth();
}

short JumpmanResources::Height() const
{
  return m_AnimRunRight.Height();
}

short JumpmanResources::Depth() const
{
  return m_AnimRunRight.Depth();

}

const AnimSeqGelsBob* JumpmanResources::AnimRunRight() const
{
  return &m_AnimRunRight;
}

const AnimSeqGelsBob* JumpmanResources::AnimRunLeft() const
{
  return &m_AnimRunLeft;
}

const AnimSeqGelsBob* JumpmanResources::AnimRunClimb1() const
{
  return &m_AnimClimb1;
}

const AnimSeqGelsBob* JumpmanResources::AnimRunClimb2() const
{
  return &m_AnimClimb2;
}
