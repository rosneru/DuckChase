#ifndef Hunter_RESOURCES_H
#define Hunter_RESOURCES_H

#include "AnimSeqGelsBob.h"
#include "GfxResourceGELs.h"

/**
 * Contains all gfx resources needed for a hunter.
 *
 *
 * @author Uwe Rosner
 * @date 30/05/2020
 */
class HunterGfxResources : public GfxResourceGELs
{
public:
  HunterGfxResources();

  WORD* DefaultImage() const;
  short Width() const;
  short WordWidth() const;
  short Height() const;
  short Depth() const;

  const AnimSeqGelsBob* AnimRightRun() const;
  const AnimSeqGelsBob* AnimLeftRun() const;
  const AnimSeqGelsBob* AnimRightShoot() const;
  const AnimSeqGelsBob* AnimLeftShoot() const;

private:
  const AnimSeqGelsBob m_AnimRightRun;
  const AnimSeqGelsBob m_AnimRightShoot;
  const AnimSeqGelsBob m_AnimLeftRun;
  const AnimSeqGelsBob m_AnimLeftShoot;
};

#endif
