#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

class ShapeBase;

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
  virtual bool Init() = 0;

  /**
   * Gets the current x-position of the shape which is associated with
   * this entity.
   */
  virtual int XPos() const;

  /**
   * Gets the current y-position of the shape which is associated with
   * this entity.
   */
  virtual int YPos() const;

protected:
  EntityBase(ShapeBase* pShape);

  ShapeBase* m_pShape;

  /**
   * Returns the distance in pixels which is calculated from the pixel-
   * per-second value und the elapsed time.
   */
  int pps2Dist(int pps, long elapsed_ms);
};

#endif
