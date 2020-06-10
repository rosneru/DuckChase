#ifndef RECT_H
#define RECT_H

class Rect
{
public:
  Rect();
  Rect(long left, long right, long top, long bottom);

  bool HasSize() const;

  long Left() const;
  long Right() const;
  long Top() const;
  long Bottom() const;

private:
  long m_Left;
  long m_Right;
  long m_Top;
  long m_Bottom;
};

#endif
