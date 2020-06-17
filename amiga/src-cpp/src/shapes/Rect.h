#ifndef RECT_H
#define RECT_H

/**
 * Represents a simple, geometric rectangle.
 *
 * @author Uwe Rosner
 * @date 10/06/2020
 */
class Rect
{
public:
  Rect();
  Rect(long left, long top, long right, long bottom);
  Rect(long left, long top);

  void Set(long left, long top, long right, long bottom);
  void SetWidthHeight(long width, long height);

  bool HasSize() const;

  long Left() const;
  long Right() const;
  long Top() const;
  long Bottom() const;
  
  long Height() const;
  long Width() const;
  long WordWidth() const;

private:
  long m_Left;
  long m_Right;
  long m_Top;
  long m_Bottom;

  long m_WordWidth;
};

#endif
