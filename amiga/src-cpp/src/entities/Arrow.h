#ifndef ARROW_H
#define ARROW_H

#include "AnimSeqExtSprite.h"
#include "Animator.h"
#include "ArrowResources.h"
#include "EntityBase.h"
#include "GameColors.h"
#include "GameViewBase.h"
#include "IlbmBitmap.h"
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
        const ArrowResources& arrowResources,
        size_t& strain,
        bool stealMouse = false);

  virtual ~Arrow();

  virtual void Activate(int x, int y, long xSpeed, long ySpeed);
  void Deactivate();

  virtual void Update(unsigned long elapsed, unsigned long joyPortState);

private:
  GameViewBase& m_GameView;
  const ArrowResources& m_Resources;

  size_t& m_Strain;

  ShapeExtSprite m_Shape;
  Animator<ShapeExtSprite, const AnimSeqExtSprite> m_Animator;

  bool m_bIsSinking;
  int m_X0;
  int m_Y0;
  int m_A;
  int m_B;
};

#endif
