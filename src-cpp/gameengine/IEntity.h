#ifndef I_ENTITY_H
#define I_ENTITY_H


/**
 * Represents an entity which populates the game world.
 *
 *
 * @author Uwe Rosner
 * @date 11/08/2019
 */
class IEntity
{
public:
  /**
   * Initialization, loading images etc.
   */
  virtual bool Init() = 0;

  /**
   * Updates this entity in the game world.
   */
  virtual void Update(unsigned long elapsed, 
                      unsigned long joyPortState) = 0;
  
  /**
   * Returns the hunter's speed in x-direction in pixel per second.
   * Negative for left movement, zero if no movement.
   */
  virtual int XSpeed_pps() const = 0;

  /**
   * Returns the hunter's speed in y-direction in pixel per second.
   * Negative for upward movement, zero if no movement.
   */
  virtual int YSpeed_pps() const = 0;

  /**
   * Gets the current y-position of the object.
   */
  virtual int XPos() const = 0;

  /**
   * Gets the current y-position of the object.
   */
  virtual int YPos() const = 0;

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
};

#endif
