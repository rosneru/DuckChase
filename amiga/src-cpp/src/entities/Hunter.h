#ifndef HUNTER_H
#define HUNTER_H


#include "Arrow.h"
#include "Animator.h"
#include "EntityBase.h"
#include "GameViewBase.h"
#include "HunterResources.h"
#include "ShapeBob.h"


/**
 * The hunter.
 *
 *
 * @author Uwe Rosner
 * @date 11/08/2019
 */
class Hunter : public EntityBase
{
public:
  Hunter(GameViewBase& gameView, 
         const GameWorld& gameWorld,
         const HunterResources& jumpmanResources,
         Arrow& arrow,
         bool& isArrowFlightStarted);

  ~Hunter();

  ShapeBase& Shape();

  virtual void Activate(int x, int y, long xSpeed, long ySpeed);
  void Deactivate();

  virtual void Update(unsigned long elapsed, unsigned long portState);

private:
  GameViewBase& m_GameView;
  const HunterResources& m_Resources;

  ShapeBob m_Shape;
  Animator<ShapeBob, const AnimSeqGelsBob> m_Animator;

  size_t m_WidthHalf;
  size_t m_WidthQuarter;

  size_t m_ElapsedSinceLastAnimUpdate;

  Arrow& m_Arrow;
  bool& m_IsArrowFlightPrepared;
  
  bool m_IsRunning;
  unsigned long m_LastDirection;

  size_t m_UpClimbingPixelsLeft;

  bool runLeft(unsigned long elapsed);
  bool runRight(unsigned long elapsed);
  void prepareArrowLaunch();
  void resetHunterActions();
};

#endif
