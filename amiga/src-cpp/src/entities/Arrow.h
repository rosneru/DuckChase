#ifndef ARROW_H
#define ARROW_H

#include "AnimSeqExtSprite.h"
#include "Animator.h"
#include "ArrowResources.h"
#include "EntityBase.h"
#include "GameColors.h"
#include "GameViewBase.h"
#include "IlbmBitMap.h"
#include "ShapeExtSprite.h"


/**
 * The arrow.
 *
 *
 * @author Uwe Rosner
 * @date 11/08/2019
 */
class Arrow : public EntityBase
{
public:
  Arrow(GameViewBase& gameView, 
        const GameWorld& gameWorld,
        const ArrowResources& arrowResources);

  virtual ~Arrow();

  virtual void Activate(int x, int y, long xSpeed_pps, long ySpeed_pps);
  virtual void Update(unsigned long elapsed, unsigned long joyPortState);

private:
  GameViewBase& m_GameView;
  const ArrowResources& m_Resources;

  ShapeExtSprite m_Shape;
  Animator<ShapeExtSprite, const AnimSeqExtSprite> m_Animator;

  size_t m_ElapsedSinceLastAnimUpdate;
};

#endif
