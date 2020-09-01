#include "DuckGfxResources.h"

DuckGfxResources::DuckGfxResources()
  : m_AnimFlyLeft("AADevDuck:assets/gfx/duck_left_flying_strip2.ilbm", 2)
{
  // TODO: Maybe check if all anims have same dimensions / depth and
  //       throw an exception if not.
}

WORD* DuckGfxResources::DefaultImage() const
{
  return m_AnimFlyLeft[0];
}

short DuckGfxResources::Width() const
{
  return m_AnimFlyLeft.Width();
}

short DuckGfxResources::WordWidth() const
{
  return m_AnimFlyLeft.WordWidth();
}

short DuckGfxResources::Height() const
{
  return m_AnimFlyLeft.Height();
}

short DuckGfxResources::Depth() const
{
  return m_AnimFlyLeft.Depth();

}

const AnimSeqGelsBob* DuckGfxResources::AnimFlyLeft() const
{
  return &m_AnimFlyLeft;
}

// const AnimSeqGelsBob* BrownBarrelResources::AnimVertically() const
// {
//   return &m_AnimVertically;
// }
