#include "GameWorld.h"

// levelData has been manually created with Excel and imported from the
// values calculated there
unsigned long levelData[] =
{
  65536,    65536,   65536,   131072,  131072,  131072,  131072,
  1048576,  1048576, 131072,  131072,  131072,  131072,  131072,
  131072,   131072,  131072,  65536,   65536,   65536,   65536,
  65536,    65536,   131072,  131072,  131072,  131072,  1048576,
  1048576,  524298,  524298,  1572874, 131072,  131072,  131072,
  131072,   131072,  65536,   65536,   65536,   65536,   65536,
  65536,    131072,  131072,  131072,  131072,  1048576, 1048576,
  131072,   131072,  1048576, 131072,  131072,  131072,  131072,
  131072,   65536,   65536,   65536,   65536,   65536,   65536,
  524295,   524295,  524295,  524295,  1572871, 9961479, 524295,
  524295,   1572871, 524296,  524297,  2621450, 524299,  131072,
  65536,    65536,   65536,   65536,   65536,   65536,   131072,
  131072,   131072,  131072,  131072,  8388608, 131072,  131072,
  131072,   131072,  131072,  2097152, 131072,  131072,  65536,
  65536,    65536,   65536,   65536,   65536,   131072,  262157,
  1310732,  262155,  262154,  8650761, 262152,  262151,  262150,
  262149,   8650756, 2359299, 262146,  262145,  65536,   65536,
  65536,    65536,   65536,   65536,   131072,  131072,  1048576,
  131072,   2097152, 131072,  131072,  131072,  131072,  131072,
  8388608,  131072,  131072,  131072,  65536,   65536,   65536,
  65536,    65536,   65536,   524290,  524291,  1572868, 524293,
  11010054, 524295,  524296,  1572873, 524298,  524299,  8912908,
  2621453,  524302,  131072,  65536,   65536,   65536,   65536,
  65536,    65536,   131072,  131072,  131072,  131072,  4194304,
  131072,   131072,  1048576, 131072,  131072,  131072,  2097152,
  131072,   131072,  65536,   65536,   65536,   65536,   65536,
  65536,    131072,  262159,  1310734, 262157,  4456460, 262155,
  1310730,  1310729, 262152,  262151,  262150,  2359301, 262148,
  262147,   65536,   65536,   65536,   65536,   65536,   65536,
  131072,   131072,  1048576, 131072,  131072,  131072,  1048576,
  131072,   131072,  131072,  131072,  131072,  131072,  131072,
  65536,    65536,   65536,   65536,   65536,   65536,   524292,
  524293,   1572870, 524295,  524296,  4718601, 1572874, 524299,
  524300,   524301,  524302,  2621455, 131072,  131072,  65536,
  65536,    65536,   65536,   65536,   65536,   131072,  131072,
  131072,   131072,  131072,  4194304, 131072,  131072,  131072,
  131072,   131072,  2097152, 524288,  131072,  65536,   65536,
  65536,    65536,   65536,   65536,   262156,  262156,  262156,
  262156,   262156,  4456460, 262156,  262155,  262154,  262153,
  262152,   2359303, 262150,  262149,  65536,   65536,   65536,
  65536,    65536,   65536,   65536,   65536,   65536,   65536,
  65536,    65536,   65536,   65536,   65536,   65536,   65536,
  65536,    65536,   65536,   65536,   65536,   65536,   65536,
  65536,    65536,   65536,   65536,   65536,   65536,   65536,
  65536,    65536,   65536,   65536,   65536,   65536,   65536,
  65536,    65536,   65536,   65536,   65536
};


GameWorld::GameWorld()
  : m_TILES_PER_ROW(20),
    m_TILE_WIDTH(16),
    m_TILE_HEIGHT(16),
    m_NumTiles(sizeof(levelData) / sizeof(levelData[0])),
    m_NumRows(m_NumTiles / m_TILES_PER_ROW),
    m_NumColumns(m_NumTiles / m_NumRows),
    m_ppTiles(new GameTile*[m_NumTiles]),
    m_BorderTile(65536, 0, 0, m_TILE_WIDTH, m_TILE_HEIGHT)
{
  size_t row = 0;
  size_t column = 0;

  for(size_t i = 0; i < m_NumTiles; i++)
  {
    m_ppTiles[i] = new GameTile(levelData[i],
                                row,
                                column++,
                                m_TILE_WIDTH,
                                m_TILE_HEIGHT);

    if(column >= m_TILES_PER_ROW)
    {
      row++;
      column = 0;
    }
  }
}

GameWorld::~GameWorld()
{
  for(size_t i = 0; i < m_NumTiles; i++)
  {
    delete m_ppTiles[i];
    m_ppTiles[i] = NULL;
  }

  delete[] m_ppTiles;
  m_ppTiles = NULL;
}

size_t GameWorld::NumTiles() const
{
  return m_NumTiles;
}

size_t GameWorld::TileId(unsigned long x, unsigned long y) const
{
  size_t _size_max = 0;
  _size_max--;

  size_t m_Row = y / m_TILE_HEIGHT;
  if(m_Row >= m_NumRows)
  {
    return _size_max;
  }

  size_t m_Column = x / m_TILE_WIDTH;
  if(m_Column >= m_NumColumns)
  {
    return _size_max;
  }

  size_t id = (m_Row * m_TILES_PER_ROW) + m_Column;
  return id;
}

const GameTile* GameWorld::Tile(unsigned long x,
                                unsigned long y,
                                bool bReturnBorderIfNull) const
{
  size_t _size_max = 0;
  _size_max--;

  size_t id = TileId(x, y);
  if(id == _size_max)
  {
    if(bReturnBorderIfNull)
    {
      return &m_BorderTile;
    }

    return NULL;
  }

  return(m_ppTiles[id]);
}

const GameTile* GameWorld::NeighborNorth(const GameTile* pTile) const
{
  if(pTile->Top() < 1)
  {
    return NULL;
  }

  const GameTile* pNorthTile = Tile(pTile->Left(),
                                    pTile->Top() - 1,
                                    false);

  return pNorthTile;
}


size_t GameWorld::CanNumPixelsLadderUp(long x, long y) const
{
  //
  // Check if current tile and ist north neighbor are ladders at all
  //
  const GameTile* pCurrentTile = Tile(x, y, false);
  if(pCurrentTile == NULL)
  {
    return 0;
  }

  if(!pCurrentTile->IsLeftLadder() && !pCurrentTile->IsRightLadder())
  {
    // An upward ladder must be on current tile
    return 0;
  }

  const GameTile* pNorthTile = NeighborNorth(pCurrentTile);
  if(pNorthTile == NULL)
  {
    return 0;
  }

  if(!pNorthTile->IsLeftLadder() && !pNorthTile->IsRightLadder())
  {
    // An upward ladder also must be on the north neighbor tile
    return 0;
  }

  //
  // Check if given x intersects the upward ladder
  //

  // Get exact as possible ladder position
  unsigned long ladderXMin = 0;
  unsigned long ladderXMax = 0;
  if(pCurrentTile->IsLeftLadder())
  {
    ladderXMin = pCurrentTile->Left();
    ladderXMax = ladderXMin + m_TILE_WIDTH / 2;
  }
  else
  {
    ladderXMax = pCurrentTile->Right();
    ladderXMin = ladderXMax - m_TILE_WIDTH / 2;
  }


  // Check if object intersects the ladder
  if(x < ladderXMin || x > ladderXMax)
  {
    return 0;
  }

  return true;
}


size_t GameWorld::PixToNextAbovePlatformTop(long x, long y) const
{
  const GameTile* pSrcTile = Tile(x, y);
  for(size_t i = 0; i < pSrcTile->Row(); i++)
  {
    // select the tile above
    const GameTile* pTile = Tile(pSrcTile->Left(),
                                 pSrcTile->Top() - ((i + 1) * m_TILE_HEIGHT));
    if(pTile->IsBorder())
    {
      return 0;
    }

    if(pTile->IsPlatformLeftSlope() || pTile->IsPlatformRightSlope())
    {
      unsigned long platformTop = pTile->PlatformTop();
      return (y - platformTop);
    }
  }

  return 0;
}
