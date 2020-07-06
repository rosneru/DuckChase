#ifndef ARROW_RESOURCES_H
#define ARROW_RESOURCES_H

#include "AnimSeqExtSprite.h"
#include "GfxResourceExtSprite.h"

/**
 * Contains all gfx resources needed for an arrow.
 *
 *
 * @author Uwe Rosner
 * @date 31/05/2020
 */
class ArrowGfxResources : public GfxResourceExtSprite
{
public:
  ArrowGfxResources();

  ExtSprite* DefaultImage() const;
  short Width() const;
  short WordWidth() const;
  short Height() const;
  short Depth() const;

  const AnimSeqExtSprite* AnimRightUpward() const;
  const AnimSeqExtSprite* AnimLeftUpward() const;

private:
  const AnimSeqExtSprite m_AnimRightUpward;
  const AnimSeqExtSprite m_AnimLeftUpward;
};

#endif
