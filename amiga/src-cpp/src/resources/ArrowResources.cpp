#include "ArrowResources.h"

ArrowResources::ArrowResources()
  : m_AnimRightUpward("AADevDuck:assets/gfx/arrow_right_strip5.ilbm", 5),
    m_AnimLeftUpward("AADevDuck:assets/gfx/arrow_left_strip5.ilbm", 5)
{
  // TODO: Maybe check if all anims have same dimensions / depth and
  //       throw an exception if not.
}

ExtSprite* ArrowResources::DefaultImage() const
{
  return m_AnimRightUpward[0];
}

short ArrowResources::Width() const
{
  return m_AnimRightUpward.Width();
}

short ArrowResources::WordWidth() const
{
  return m_AnimRightUpward.WordWidth();
}

short ArrowResources::Height() const
{
  return m_AnimRightUpward.Height();
}

short ArrowResources::Depth() const
{
  return m_AnimRightUpward.Depth();

}

const AnimSeqExtSprite* ArrowResources::AnimRightUpward() const
{
  return &m_AnimRightUpward;
}

const AnimSeqExtSprite* ArrowResources::AnimLeftUpward() const
{
  return &m_AnimLeftUpward;
}
