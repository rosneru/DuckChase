#include "GameTile.h"


GameTile::GameTile(long properties, 
                   long row, 
                   long column,
                   const long tileWidth,
                   const long tileHeight)
  : m_Row(row),
    m_Column(column),
    m_Left(column * tileWidth),
    m_Right(((column + 1) * tileWidth) - 1),
    m_Top(row * tileHeight),
    m_Bottom(((row + 1) * tileHeight) - 1),
    m_PlatformTop((row * tileHeight) + (properties & 0xff)),
    m_PlatformHeightLimit((properties & 0xff00) >> 8),
    m_bIsBorder(((properties & 0xffff0000) >> 16) & 1),
    m_bIsEmpty(((properties & 0xffff0000) >> 16) & 2),
    m_bIsPlatformLeftSlope(((properties & 0xffff0000) >> 16) & 4),
    m_bIsPlatformRightSlope(((properties & 0xffff0000) >> 16) & 8),
    m_bIsLeftLadder(((properties & 0xffff0000) >> 16) & 16),
    m_bIsRightLadder(((properties & 0xffff0000) >> 16) & 32),
    m_bIsLeftBrokenLadder(((properties & 0xffff0000) >> 16) & 64),
    m_bIsRightBrokenLadder(((properties & 0xffff0000) >> 16) & 128)
{

}

long GameTile::Row() const
{
  return m_Row;
}

long GameTile::Column() const
{
  return m_Column;
}

long GameTile::PlatformTop() const
{
  return m_PlatformTop;
}

long GameTile::PlatformHeightLimit() const
{
  return m_PlatformHeightLimit;
}

long GameTile::Left() const
{
  return m_Left;
}

long GameTile::Right() const
{
  return m_Right;
}

long GameTile::Top() const
{
  return m_Top;
}

long GameTile::Bottom() const
{
  return m_Bottom;
}

bool GameTile::IsBorder () const
{
  return m_bIsBorder;
}


bool GameTile::IsEmpty () const
{
  return m_bIsEmpty;
}


bool GameTile::IsPlatformLeftSlope () const
{
  return m_bIsPlatformLeftSlope;
}


bool GameTile::IsPlatformRightSlope () const
{
  return m_bIsPlatformRightSlope;
}


bool GameTile::IsLeftLadder () const
{
  return m_bIsLeftLadder;
}


bool GameTile::IsRightLadder () const
{
  return m_bIsRightLadder;
}


bool GameTile::IsLeftBrokenLadder () const
{
  return m_bIsLeftBrokenLadder;
}


bool GameTile::IsRightBrokenLadder () const
{
  return m_bIsRightBrokenLadder;
}
