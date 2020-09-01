#include "ArrowGfxResources.h"

ArrowGfxResources::ArrowGfxResources()
  : m_AnimRightUpward("AADevDuck:assets/gfx/arrow_right_strip5.ilbm", 5),
    m_AnimLeftUpward("AADevDuck:assets/gfx/arrow_left_strip5.ilbm", 5)
{
  // TODO: Maybe check if all anims have same dimensions / depth and
  //       throw an exception if not.
}

ExtSprite* ArrowGfxResources::DefaultImage() const
{
  return m_AnimRightUpward[0];
}

short ArrowGfxResources::Width() const
{
  return m_AnimRightUpward.Width();
}

short ArrowGfxResources::WordWidth() const
{
  return m_AnimRightUpward.WordWidth();
}

short ArrowGfxResources::Height() const
{
  return m_AnimRightUpward.Height();
}

short ArrowGfxResources::Depth() const
{
  return m_AnimRightUpward.Depth();

}

const AnimSeqExtSprite* ArrowGfxResources::AnimRightUpward() const
{
  return &m_AnimRightUpward;
}

const AnimSeqExtSprite* ArrowGfxResources::AnimLeftUpward() const
{
  return &m_AnimLeftUpward;
}
