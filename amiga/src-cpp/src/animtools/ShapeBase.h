#ifndef SHAPE_BASE_H
#define SHAPE_BASE_H

class AnimSeqBase;

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

  /**
   * Disables the double buffering for this shape. Must be set before
   * init.
   *
   * NOTE: Not all shape types may be affected, because not all of them
   * use double buffering at all, e.g. Bobs can be double buffered,
   * Sprites not.
   */
  virtual void DisableDoubleBuf();

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
   * Set a sequence of images to the anim object which displayed as the
   * shapes main content. It can be animated by calling \ref NextImage.
   */
  virtual void SetAnimSequence(AnimSeqBase* pAnimSequence) = 0;

  /**
   * Set the shape to display the next anim image. It's done circular:
   * After the last image the first image is displayed again.
   */
  virtual void NextImage() = 0;

protected:
  ShapeBase();

  bool m_IsDoubleBuffered;
};

#endif
