#ifndef SHAPE_BASE_H
#define SHAPE_BASE_H

#include <stddef.h>

#include "IlbmBitmap.h"
#include "Rect.h"


/**
 * Represents a shape, an graphical object which can be moved over the
 * playfield without destryoing the background.
 *
 *
 * @author Uwe Rosner
 * @date 11/08/2019
 */
class ShapeBase
{
public:
  /**
   * Moves the object by its HotSpot to the given position. As per 
   * default the HotSpot is set to the top, left edge of the object. 
   *
   * Calls the move() method which each derived class must implement.
   */
  void Move(int x, int y);

  /**
   * Gets the width of the object
   */
  short Width() const ;

  /**
   * Gets the height of the object
   */
  short Height() const;

  /**
   * Returns the shape x-position on the screen relative to its
   * hotspot. By default hotspot (0, 0) this is the left edge of the 
   * shape.
   */
  int X() const;

  /**
   * Returns the shape y-position on the screen relative to its
   * hotspot. By default hotspot (0, 0) this is the left edge of the 
   * shape.
   */
  int Y() const;

  /**
   * Set the hostpot inside the shape. Actually can even be outside the
   * shape.
   */
  void SetHotspot(int hotspotX, int hotspotY);


  /**
   * Returns the current left edge position of the object.
   * 
   * NOTE: Returns the real position of the shape, doesn't take the 
   * hotspot into account.
   */
  virtual int Left() const = 0;

  /**
   * Returns the current top edge position of the object.
   * 
   * NOTE: Returns the real position of the shape, doesn't take the 
   * hotspot into account.
   */
  virtual int Top() const = 0;

  /**
   * Returns the current right edge position of the object.
   * 
   * NOTE: Returns the real position of the shape, doesn't take the 
   * hotspot into account.
   */
  virtual int Right() const = 0;

  /**
   * Returns the current bottom edge position of the object.
   * 
   * NOTE: Returns the real position of the shape, doesn't take the 
   * hotspot into account.
   */
  virtual int Bottom() const = 0;


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
   * Returns true if this shape isn't on display anymore
   */
  virtual bool IsGone() const = 0;


  /**
   * Returns true if this object intersects / collides with the other
   * object. In collision case it calculates the collision rect.
   */
  bool Intersects(const ShapeBase& other);

  const Rect& IntersectRect() const;


  enum BottomDistanceMeasMode
  {
    BDMM_Left,
    BDMM_Middle,
    BDMM_Right,
    BDMM_All
  };

  /**
   * Returns the minimum y-distance of the leftmost, middle and right 
   * point of the current shape below to the next platform on given 
   * background picture. Searches the next dY pixels below the shape for 
   * the given color number (which should be the topmost color of the 
   * platform)
   *
   * Returns -1 if given color is not found withing dY pixels below the 
   * shape.
   */
  int BottomToPlatformDistance(IlbmBitmap& picture, 
                               int dY, 
                               int searchedColorNum,
                               BottomDistanceMeasMode mode);

protected:
  ShapeBase(short width, short height, short depth);

  /**
   * The actual move method; it has to be implemented by the childs.
   */
  virtual void move(int x, int y) = 0;

  short m_Width;
  short m_WordWidth;
  short m_Height;
  short m_Depth;

private:
  int m_HotspotX;
  int m_HotspotY;
  Rect m_IntersRect;
};

#endif
