#ifndef BROWN_BARREL_H
#define BROWN_BARREL_H

#include "AnimSeqGelsBob.h"
#include "Animator.h"
#include "BrownBarrelResources.h"
#include "EntityBase.h"
#include "GameColors.h"
#include "GameViewBase.h"
#include "IlbmBitMap.h"
#include "ShapeBob.h"


/**
 * A brown barrel.
 *
 *
 * @author Uwe Rosner
 * @date 29/02/2020
 */
class BrownBarrel : public EntityBase
{
public:
  BrownBarrel(GameViewBase& gameView, 
              const GameWorld& gameWorld,
              const BrownBarrelResources& brownBarrelResources);

  virtual ~BrownBarrel();

  virtual void Activate(int x, int y, long xSpeed_pps, long ySpeed_pps);
  virtual void Update(unsigned long elapsed, unsigned long joyPortState);

private:
  GameViewBase& m_GameView;
  const BrownBarrelResources& m_Resources;

  ShapeBob m_Shape;
  Animator<ShapeBob, const AnimSeqGelsBob> m_Animator;

  int m_AnimFrameCnt;
};

#endif
