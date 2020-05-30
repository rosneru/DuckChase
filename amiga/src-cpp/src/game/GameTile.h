#ifndef DONKEYKONG_TILE_H
#define DONKEYKONG_TILE_H

#include <stddef.h>

/**
 * A block as tile of the DonkeyKong GameWorld.
 *
 * @author Uwe Rosner
 * @date 03/05/2020
 */
class GameTile
{
public:
  GameTile(unsigned long properties, 
           size_t row, 
           size_t column, 
           size_t tileWidth,
           size_t tileHeight);

  /**
   * Returns the table row of this tile
   */
  size_t Row() const;

  /**
   * Returns the table column of this tile
   */
  size_t Column() const;

  /**
   * Top-position of the platform in pixel. Measured from top of the
   * GameWorld.
   */
  unsigned long PlatformTop() const;

  /**
   * Number of pixels from the top of the tile that can't be visited
   * by an entity because it containts a wall etc.
   * 
   * This number reduces the available height above the platform.
   * 
   * Defaults to 0.
   */
  unsigned long PlatformHeightLimit() const;

  unsigned long Left() const;
  unsigned long Right() const;
  unsigned long Top() const;
  unsigned long Bottom() const;

  bool IsBorder() const;
  bool IsEmpty() const;
  bool IsPlatformLeftSlope() const;
  bool IsPlatformRightSlope() const;
  bool IsLeftLadder() const;
  bool IsRightLadder() const;
  bool IsLeftBrokenLadder() const;
  bool IsRightBrokenLadder() const;


private:
  size_t m_Row;
  size_t m_Column;

  unsigned long m_Left;
  unsigned long m_Right;
  unsigned long m_Top;
  unsigned long m_Bottom;

  unsigned long m_PlatformTop;
  unsigned long m_PlatformHeightLimit;
  bool m_bIsBorder;
  bool m_bIsEmpty;
  bool m_bIsPlatformLeftSlope;
  bool m_bIsPlatformRightSlope;
  bool m_bIsLeftLadder;
  bool m_bIsRightLadder;
  bool m_bIsLeftBrokenLadder;
  bool m_bIsRightBrokenLadder;
};

#endif
