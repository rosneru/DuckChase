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
  GameTile(long properties, 
           long row, 
           long column, 
           long tileWidth,
           long tileHeight);

  /**
   * Returns the table row of this tile
   */
  long Row() const;

  /**
   * Returns the table column of this tile
   */
  long Column() const;

  /**
   * Top-position of the platform in pixel. Measured from top of the
   * GameWorld.
   */
  long PlatformTop() const;

  /**
   * Number of pixels from the top of the tile that can't be visited
   * by an entity because it containts a wall etc.
   * 
   * This number reduces the available height above the platform.
   * 
   * Defaults to 0.
   */
  long PlatformHeightLimit() const;

  long Left() const;
  long Right() const;
  long Top() const;
  long Bottom() const;

  bool IsBorder() const;
  bool IsEmpty() const;
  bool IsPlatformLeftSlope() const;
  bool IsPlatformRightSlope() const;
  bool IsLeftLadder() const;
  bool IsRightLadder() const;
  bool IsLeftBrokenLadder() const;
  bool IsRightBrokenLadder() const;


private:
  long m_Row;
  long m_Column;

  long m_Left;
  long m_Right;
  long m_Top;
  long m_Bottom;

  long m_PlatformTop;
  long m_PlatformHeightLimit;
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
