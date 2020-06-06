#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

#include <stddef.h>

#include "GameWorld.h"
#include "ShapeBase.h"

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
  /**
   * Returns the objects current speed in x-direction in pixel per second.
   */
  virtual long XSpeed();

  /**
   * Returns the objects current speed in y-direction in pixel per second.
   */
  virtual long YSpeed();
  
  /**
   * Returns if the object is still alive.
   */
  virtual bool IsAlive();

  /**
   * Activate the object at given position with given speed. Speed unit
   * is pixel per second.
   */
  virtual void Activate(int x, int y, long xSpeed, long ySpeed) = 0;

  /**
   * Deactivate the object.
   */
  virtual void Deactivate() = 0;

  /**
   * Update the object.
   */
  virtual void Update(unsigned long elapsed, unsigned long joyPortState) = 0;

  virtual ShapeBase& Shape() = 0;

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

  long m_XSpeed;
  long m_XSpeedInitial;
  long m_YSpeed;
  long m_YSpeedInitial;

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
