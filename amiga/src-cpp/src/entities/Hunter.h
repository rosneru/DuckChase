#ifndef JUMPMAN_H
#define JUMPMAN_H


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
          bool& isArrowLaunching, 
          bool& isArrowLaunchDone);

  ~Hunter();

  virtual void Activate(int x, int y, long xSpeed_pps, long ySpeed_pps);
  virtual void Update(unsigned long elapsed, unsigned long portState);

private:
  GameViewBase& m_GameView;
  const HunterResources& m_Resources;

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
  bool launchArrow();
  void resetHunterActions();
};

#endif
