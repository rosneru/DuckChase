#include "Rect.h"

Rect::Rect()
  : m_Left(0),
    m_Right(0),
    m_Top(0),
    m_Bottom(0)
{

}

Rect::Rect(long left, long right, long top, long bottom)
  : m_Left(left),
    m_Right(right),
    m_Top(top),
    m_Bottom(bottom)
{

}


bool Rect::HasSize() const
{
  return (m_Left != m_Right) && (m_Top != m_Bottom);
}


long Rect::Left() const
{
  return m_Left;
}


long Rect::Right() const
{
  return m_Right;
}


long Rect::Top() const
{
  return m_Top;
}


long Rect::Bottom() const
{
  return m_Bottom;
}
