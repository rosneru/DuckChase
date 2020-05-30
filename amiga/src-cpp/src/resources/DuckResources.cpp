#include "DuckResources.h"

DuckResources::DuckResources()
  : m_AnimHorizontally("AADevDuck:assets/barrel_bn_roll_right_strip4.ilbm", 4)
    // m_AnimVertically("AADevDuck:assets/barrel_bn_roll_down_strip2.ilbm", 2),

{
  // TODO: Maybe check if all anims have same dimensions / depth and
  //       throw one exception if not.
}

WORD* DuckResources::DefaultImage() const
{
  return m_AnimHorizontally[0];
}

short DuckResources::Width() const
{
  return m_AnimHorizontally.Width();
}

short DuckResources::WordWidth() const
{
  return m_AnimHorizontally.WordWidth();
}

short DuckResources::Height() const
{
  return m_AnimHorizontally.Height();
}

short DuckResources::Depth() const
{
  return m_AnimHorizontally.Depth();

}

const AnimSeqGelsBob* DuckResources::AnimHorizontally() const
{
  return &m_AnimHorizontally;
}

// const AnimSeqGelsBob* BrownBarrelResources::AnimVertically() const
// {
//   return &m_AnimVertically;
// }
