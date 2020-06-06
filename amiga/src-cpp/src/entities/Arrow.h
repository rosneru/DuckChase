#ifndef ARROW_H
#define ARROW_H

#include "AnimSeqExtSprite.h"
#include "Animator.h"
#include "ArrowResources.h"
#include "Duck.h"
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
        Duck& duck,
        size_t& strain,
        bool& strike,
        bool stealMouse = false);

  virtual ~Arrow();
  
  ShapeBase& Shape();

  virtual void Activate(int x, int y, long xSpeed, long ySpeed);
  void Deactivate();

  virtual void Update(unsigned long elapsed, unsigned long joyPortState);


private:
  GameViewBase& m_GameView;
  const ArrowResources& m_Resources;

  Duck& m_Duck;
  size_t& m_Strain;
  bool& m_Strike;

  ShapeExtSprite m_Shape;
  Animator<ShapeExtSprite, const AnimSeqExtSprite> m_Animator;

  int m_X0;
  int m_Y0;
  int m_A;
  int m_B;
};

#endif
