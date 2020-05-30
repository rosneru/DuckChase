#ifndef JUMPMAN_H
#define JUMPMAN_H


#include "Animator.h"
#include "EntityBase.h"
#include "GameViewBase.h"
#include "JumpmanResources.h"
#include "ShapeBob.h"


/**
 * The Jumpman.
 *
 *
 * @author Uwe Rosner
 * @date 29/02/2020
 */
class Jumpman : public EntityBase
{
public:
  Jumpman(GameViewBase& gameView, 
          const GameWorld& gameWorld,
          const JumpmanResources& jumpmanResources,
          bool& isArrowLaunching, 
          bool& isArrowLaunchDone);

  ~Jumpman();

  virtual void Activate(int x, int y, long xSpeed_pps, long ySpeed_pps);
  virtual void Update(unsigned long elapsed, unsigned long portState);

private:
  GameViewBase& m_GameView;
  const JumpmanResources& m_Resources;

  ShapeBob m_Shape;
  Animator<ShapeBob, const AnimSeqGelsBob> m_Animator;

  size_t m_WidthHalf;
  size_t m_WidthQuarter;

  size_t m_ElapsedSinceLastAnimUpdate;

  bool& m_IsArrowLaunched;
  bool m_IsLaunchingArrow;
  bool m_IsRunning;
  unsigned long m_LastDirection;

  size_t m_UpClimbingPixelsLeft;

  bool runLeft(unsigned long elapsed);
  bool runRight(unsigned long elapsed);
  bool climbLadderUp(unsigned long elapsed);
};

#endif
