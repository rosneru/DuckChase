#include "DuckResources.h"

DuckResources::DuckResources()
  : m_AnimFlyLeft("AADevDuck:assets/duck_left_flying_strip2.iff", 2)
{
  // TODO: Maybe check if all anims have same dimensions / depth and
  //       throw one exception if not.
}

WORD* DuckResources::DefaultImage() const
{
  return m_AnimFlyLeft[0];
}

short DuckResources::Width() const
{
  return m_AnimFlyLeft.Width();
}

short DuckResources::WordWidth() const
{
  return m_AnimFlyLeft.WordWidth();
}

short DuckResources::Height() const
{
  return m_AnimFlyLeft.Height();
}

short DuckResources::Depth() const
{
  return m_AnimFlyLeft.Depth();

}

const AnimSeqGelsBob* DuckResources::AnimFlyLeft() const
{
  return &m_AnimFlyLeft;
}

// const AnimSeqGelsBob* BrownBarrelResources::AnimVertically() const
// {
//   return &m_AnimVertically;
// }
