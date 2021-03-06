#include "OpenIlbmPictureBitMap.h"
#include "ShapeBase.h"

ShapeBase::ShapeBase(short width, short height, short depth)
  : m_Width(width),
    m_WordWidth(((m_Width + 15) & -16) >> 4),
    m_Height(height),
    m_Depth(depth),
    m_HotspotX(0),
    m_HotspotY(0)
{

}


void ShapeBase::Move(int x, int y)
{
  move(x - m_HotspotX, y - m_HotspotY);
}


short ShapeBase::Width() const
{
  return m_Width;
}


short ShapeBase::Height() const
{
  return m_Height;
}


int ShapeBase::X() const
{
  return Left() + m_HotspotX;
}


int ShapeBase::Y() const
{
  return Top() + m_HotspotY;
}


void ShapeBase::SetHotspot(int hotspotX, int hotspotY)
{
  m_HotspotX = hotspotX;
  m_HotspotY = hotspotY;
}


bool ShapeBase::Intersects(const ShapeBase &other)
{
  if(Left() < other.Right() && 
     Right() > other.Left() && 
     Top() < other.Bottom() && 
     Bottom() > other.Top())
  {
    m_IntersRect.Set(other.Left() > Left() ? other.Left()-Left() : 0, 
                     other.Top() > Top() ? other.Top()-Top() : 0, 
                     other.Right() < Right() ? other.Right()-Left() : Width()-1, 
                     other.Bottom() < Bottom() ? other.Bottom()-Top() : Height()-1);

    return true;
  }
  else
  {
    return false;
  }
}


const Rect& ShapeBase::IntersectRect() const
{
  return m_IntersRect;
}


int ShapeBase::BottomToPlatformDistance(OpenIlbmPictureBitMap& picture, 
                                        int dY, 
                                        int searchedColorNum,
                                        BottomDistanceMeasMode mode)
{
  ULONG shapeBottom = Top() + Height();
  ULONG xLeft = Left();
  ULONG xMid = xLeft + (Width() / 2);
  ULONG xRight = xLeft + Width() - 1;

  for(int distance = 0; distance < dY; distance++)
  {
    int y = shapeBottom + distance;

    if(mode == BDMM_Left || mode == BDMM_All)
    {
      int colorNum = picture.GetBitMapPixelColorNum(y, xLeft);
      if(colorNum == searchedColorNum)
      {
        return distance;
      }
    }

    if(mode == BDMM_Middle || mode == BDMM_All)
    {
      int colorNum = picture.GetBitMapPixelColorNum(y, xMid);
      if(colorNum == searchedColorNum)
      {
        return distance;
      }
    }

    if(mode == BDMM_Right || mode == BDMM_All)
    {
      int colorNum = picture.GetBitMapPixelColorNum(y, xRight);
      if(colorNum == searchedColorNum)
      {
        return distance;
      }
    }
  }

  return -1;
}
