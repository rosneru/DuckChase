#include "IlbmBitMap.h"
#include "ShapeBase.h"

ShapeBase::ShapeBase(const ResourceBase& gfxResources)
  : m_IsDoubleBuffered(true),
    m_Width(gfxResources.Width()),
    m_WordWidth(gfxResources.WordWidth()),
    m_Height(gfxResources.Height()),
    m_Depth(gfxResources.Depth()),
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


int ShapeBase::X()
{
  return Left() + m_HotspotX;
}


int ShapeBase::Y()
{
  return Top() + m_HotspotY;
}

void ShapeBase::SetHotspot(int hotspotX, int hotspotY)
{
  m_HotspotX = hotspotX;
  m_HotspotY = hotspotY;
}


void ShapeBase::DisableDoubleBuf()
{
  m_IsDoubleBuffered = false;
}

int ShapeBase::BottomToPlatformDistance(IlbmBitMap& picture, 
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
