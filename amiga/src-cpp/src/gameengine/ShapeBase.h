#ifndef SHAPE_BASE_H
#define SHAPE_BASE_H


/**
 * Represents a shape, an graphical object which can be moved above the
 * background without distracting it.
 *
 *
 * @author Uwe Rosner
 * @date 11/08/2019
 */
class ShapeBase
{
public:
  // /**
  //  * Updates this entity in the game world.
  //  */
  // virtual void Update(unsigned long elapsed,
  //                     unsigned long joyPortState) = 0;

  /**
   * Returns the hunter's speed in x-direction in pixel per second.
   * Negative for left movement, zero if no movement.
   */
//  virtual int XSpeed_pps() const = 0;

  /**
   * Returns the hunter's speed in y-direction in pixel per second.
   * Negative for upward movement, zero if no movement.
   */
//  virtual int YSpeed_pps() const = 0;

  /**
   * Gets the current y-position of the object.
   */
  virtual int XPos() const = 0;

  /**
   * Gets the current y-position of the object.
   */
  virtual int YPos() const = 0;

  /**
   * Gets the width of the object
   */
  virtual int Width() const = 0;

  /**
   * Gets the height of the object
   */
  virtual int Height() const = 0;



  /**
   * Moves the object to the desired position
   */
  virtual void Move(int x, int y) = 0;

  /**
   * Sets the object invisible and stops the animation.
   */
  virtual void SetInvisible() = 0;

  /**
   * Sets the object visible and continues the animation.
   */
  virtual void SetVisible() = 0;

  /**
   * Returns if the object currently is visible.
   */
  virtual bool IsVisible() const = 0;

  /**
   * Sets the next anim image to the object. After the last image
   * the first image is displayed again.
   */
  virtual void NextImage() = 0;

protected:
  /**
   * Returns the distance in pixels which is calculated from the pixel-
   * per-second value und the elapsed time.
   */
  int pps2Dist(int pps, long elapsed_ms);
};

#endif
