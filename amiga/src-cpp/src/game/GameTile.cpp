#include "GameTile.h"


GameTile::GameTile(unsigned long properties, 
                   size_t row, 
                   size_t column,
                   const size_t tileWidth,
                   const size_t tileHeight)
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

size_t GameTile::Row() const
{
  return m_Row;
}

size_t GameTile::Column() const
{
  return m_Column;
}

unsigned long GameTile::PlatformTop() const
{
  return m_PlatformTop;
}

unsigned long GameTile::PlatformHeightLimit() const
{
  return m_PlatformHeightLimit;
}

unsigned long GameTile::Left() const
{
  return m_Left;
}

unsigned long GameTile::Right() const
{
  return m_Right;
}

unsigned long GameTile::Top() const
{
  return m_Top;
}

unsigned long GameTile::Bottom() const
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
