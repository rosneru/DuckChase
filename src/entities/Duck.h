#ifndef DUCK_H
#define DUCK_H

#include "AnimSeqGelsBob.h"
#include "Animator.h"
#include "DuckGfxResources.h"
#include "EntityBase.h"
#include "GameColors.h"
#include "GameViewBase.h"
#include "OpenIlbmPictureBitMap.h"
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
       const DuckGfxResources& duckResources);

  virtual ~Duck();

  void Activate(int x, int y, long xSpeed, long ySpeed);
  void Deactivate();
  
  void Update(unsigned long elapsed, unsigned long joyPortState);

private:
  GameViewBase& m_GameView;
  const DuckGfxResources& m_Resources;

  ShapeBob m_DuckShape;
  Animator<ShapeBob, const AnimSeqGelsBob> m_Animator;

  size_t m_ElapsedSinceLastAnimUpdate;
};

#endif
