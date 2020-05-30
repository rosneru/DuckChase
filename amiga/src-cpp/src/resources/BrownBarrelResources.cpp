#include "BrownBarrelResources.h"

BrownBarrelResources::BrownBarrelResources()
  : m_AnimHorizontally("gfx/barrel_bn_roll_right_strip4.ilbm", 4)
    // m_AnimVertically("gfx/barrel_bn_roll_down_strip2.ilbm", 2),

{
  // TODO: Maybe check if all anims have same dimeansions / depth and
  //       throw one exception if not.
}

WORD* BrownBarrelResources::DefaultImage() const
{
  return m_AnimHorizontally[0];
}

short BrownBarrelResources::Width() const
{
  return m_AnimHorizontally.Width();
}

short BrownBarrelResources::WordWidth() const
{
  return m_AnimHorizontally.WordWidth();
}

short BrownBarrelResources::Height() const
{
  return m_AnimHorizontally.Height();
}

short BrownBarrelResources::Depth() const
{
  return m_AnimHorizontally.Depth();

}

const AnimSeqGelsBob* BrownBarrelResources::AnimHorizontally() const
{
  return &m_AnimHorizontally;
}

// const AnimSeqGelsBob* BrownBarrelResources::AnimVertically() const
// {
//   return &m_AnimVertically;
// }
