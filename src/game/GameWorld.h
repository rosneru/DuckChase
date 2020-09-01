#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include <stddef.h>
#include "GameTile.h"

/**
 * The DonkeyKong GameWorld which consists of many tiles of 16x16 size.
 *
 * @author Uwe Rosner
 * @date 03/05/2020
 */
class GameWorld
{
public:
  GameWorld();
  virtual ~GameWorld();

  /**
   * Returns the number of tiles in GameWorld
   */
  size_t NumTiles() const;

  /**
   * Returns the tile at given pixel position or __SIZE_MAX__ if there
   * is none.
   */

  size_t TileId(unsigned long x, unsigned long y) const;

  /**
   * Returns the tile at given pixel position or NULL if there is none.
   */
  const GameTile* Tile(unsigned long x, 
                       unsigned long y, 
                       bool bReturnBorderIfNull = true) const;

  /**
   * Retuns the tile thats the north neigbor of the given one.
   */
  const GameTile* NeighborNorth(const GameTile* pTile) const;
  
  /**
   * Returns true if on given position an upward letter exists on which
   * an entity can climb up.
   * 
   * The tile at given position and its north neighbor are checked to 
   * get this information.
   */
  size_t CanNumPixelsLadderUp(long x, long y) const;


  /**
   * Returns the number of pixels from given y-Position to the next
   * platform top abbove tile at given position.
   * 
   * Returns 0 if there is no platform top on any tile above.
   */
  size_t PixToNextAbovePlatformTop(long x, long y) const;


private:
  const size_t m_TILES_PER_ROW;
  const size_t m_TILE_WIDTH;
  const size_t m_TILE_HEIGHT;
  const size_t m_NumTiles;
  const size_t m_NumRows;
  const size_t m_NumColumns;
  GameTile** m_ppTiles;

  GameTile m_BorderTile; // To be returned when limits exaggerated

};

#endif
