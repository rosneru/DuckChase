#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

#include <stddef.h>

#include "GameWorld.h"

/**
 * Represents a shape, an graphical object which can be moved above the
 * background without distracting it.
 *
 *
 * @author Uwe Rosner
 * @date 18/01/2020
 */
class EntityBase
{
public:
  virtual long XSpeed_pps();
  virtual long YSpeed_pps();
  
  virtual bool IsAlive();


  virtual void Activate(int x, int y, long xSpeed_pps = 0, long ySpeed_pps = 0) = 0;
protected:
  const GameWorld& m_GameWorld;

  enum Actions
  {
    MoveHorizontally,
    MoveDiagonally,
    MoveVertically,
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    JumpUp,
    JumpUpRight,
    JumpRight,
    JumpUpLeft,
    JumpLeft,
    StandAffront,
    StandAverted,
  };

  Actions m_Action;

  long m_XSpeed_pps;
  long m_YSpeed_pps;

  bool m_bIsAlive;

  int m_FrameSwitchingRateAt50Fps;

  EntityBase(const GameWorld& gameWorld);


  /**
   * Returns the distance in pixels which is calculated from the pixel-
   * per-second value und the elapsed time.
   */
  long pps2Dist(long pps, long elapsed_ms);
};

#endif
