#ifndef ARROW_H
#define ARROW_H

#include "AnimSeqExtSprite.h"
#include "Animator.h"
#include "ArrowGfxResources.h"
#include "Duck.h"
#include "EntityBase.h"
#include "GameColors.h"
#include "GameViewBase.h"
#include "OpenIlbmPictureBitMap.h"
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
        const ArrowGfxResources& arrowResources,
        Duck& duck,
        size_t& strain,
        bool& isArrowFlightFinished,
        bool& isStrike);

  virtual ~Arrow();

  virtual void Activate(int x, int y, long xSpeed, long ySpeed);
  void Deactivate();

  virtual void Update(unsigned long elapsed, unsigned long joyPortState);


private:
  GameViewBase& m_GameView;
  const ArrowGfxResources& m_Resources;

  Duck& m_Duck;
  size_t& m_Strain;
  bool& m_IsArrowFlightFinished;
  bool& m_IsStrike;

  ShapeExtSprite m_ArrowShape;
  Animator<ShapeExtSprite, const AnimSeqExtSprite> m_Animator;

  int m_X0;
  int m_Y0;
  int m_A;
  int m_B;
};

#endif
