#ifndef BROWN_BARREL_H
#define BROWN_BARREL_H

#include "AnimSeqGelsBob.h"
#include "Animator.h"
#include "DuckResources.h"
#include "EntityBase.h"
#include "GameColors.h"
#include "GameViewBase.h"
#include "IlbmBitMap.h"
#include "ShapeBob.h"


/**
 * The duck.
 *
 *
 * @author Uwe Rosner
 * @date 11/08/2019
 */
class Duck : public EntityBase
{
public:
  Duck(GameViewBase& gameView, 
       const GameWorld& gameWorld,
       const DuckResources& duckResources);

  virtual ~Duck();

  virtual void Activate(int x, int y, long xSpeed_pps, long ySpeed_pps);
  virtual void Update(unsigned long elapsed, unsigned long joyPortState);

private:
  GameViewBase& m_GameView;
  const DuckResources& m_Resources;

  ShapeBob m_Shape;
  Animator<ShapeBob, const AnimSeqGelsBob> m_Animator;

  size_t m_ElapsedSinceLastAnimUpdate;
};

#endif
