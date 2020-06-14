#include "GameColors.h"


/**
 * The 3 colors for the arrow sprite
 */
GameColors::BarrelBrownSpriteColors GameColors::m_sArrowSpriteColors =
{
  0x0fb9, 0x000f, 0x0e71
};


GameColors::StrainSpreadColors GameColors::m_sStrainSpreadColors = 
{
  0x83000000, 0xA5000000, 0x98000000,
  0x83000000, 0xA6000000, 0x97000000,
  0x83000000, 0xA7000000, 0x96000000,
  0x82000000, 0xA9000000, 0x95000000,
  0x82000000, 0xAA000000, 0x94000000,
  0x81000000, 0xAB000000, 0x92000000,
  0x81000000, 0xAD000000, 0x91000000,
  0x80000000, 0xAE000000, 0x8F000000,
  0x7F000000, 0xAF000000, 0x8D000000,
  0x7F000000, 0xB1000000, 0x8B000000,
  0x7E000000, 0xB2000000, 0x89000000,
  0x7D000000, 0xB4000000, 0x86000000,
  0x7D000000, 0xB5000000, 0x84000000,
  0x7C000000, 0xB6000000, 0x81000000,
  0x7B000000, 0xB8000000, 0x7E000000,
  0x7A000000, 0xB9000000, 0x7B000000,
  0x7A000000, 0xBA000000, 0x79000000,
  0x7C000000, 0xBC000000, 0x79000000,
  0x7F000000, 0xBD000000, 0x78000000,
  0x7F000000, 0xBE000000, 0x76000000,
  0x83000000, 0xC0000000, 0x76000000,
  0x84000000, 0xC1000000, 0x74000000,
  0x88000000, 0xC3000000, 0x74000000,
  0x8B000000, 0xC4000000, 0x73000000,
  0x8D000000, 0xC5000000, 0x71000000,
  0x91000000, 0xC7000000, 0x71000000,
  0x94000000, 0xC8000000, 0x70000000,
  0x97000000, 0xC9000000, 0x6E000000,
  0x9C000000, 0xCB000000, 0x6E000000,
  0xA0000000, 0xCC000000, 0x6C000000,
  0xA3000000, 0xCD000000, 0x6B000000,
  0xA8000000, 0xCF000000, 0x6A000000,
  0xAC000000, 0xD0000000, 0x68000000,
  0xB1000000, 0xD2000000, 0x67000000,
  0xB6000000, 0xD3000000, 0x66000000,
  0xBB000000, 0xD4000000, 0x64000000,
  0xC2000000, 0xD6000000, 0x64000000,
  0xC6000000, 0xD7000000, 0x61000000,
  0xCB000000, 0xD8000000, 0x60000000,
  0xD2000000, 0xDA000000, 0x5F000000,
  0xD7000000, 0xDB000000, 0x5D000000,
  0xDC000000, 0xD9000000, 0x5C000000,
  0xDE000000, 0xD5000000, 0x5A000000,
  0xDF000000, 0xD2000000, 0x59000000,
  0xE1000000, 0xCE000000, 0x58000000,
  0xE2000000, 0xC9000000, 0x56000000,
  0xE3000000, 0xC3000000, 0x54000000,
  0xE5000000, 0xBF000000, 0x53000000,
  0xE6000000, 0xB9000000, 0x51000000,
  0xE7000000, 0xB3000000, 0x4F000000,
  0xE9000000, 0xAE000000, 0x4D000000,
  0xEA000000, 0xA8000000, 0x4C000000,
  0xEB000000, 0xA1000000, 0x4A000000,
  0xED000000, 0x9A000000, 0x48000000,
  0xEE000000, 0x93000000, 0x46000000,
  0xF0000000, 0x8D000000, 0x45000000,
  0xF1000000, 0x85000000, 0x43000000,
  0xF2000000, 0x7D000000, 0x41000000,
  0xF4000000, 0x75000000, 0x3F000000,
  0xF5000000, 0x6D000000, 0x3D000000,
  0xF6000000, 0x64000000, 0x3B000000,
  0xF8000000, 0x5B000000, 0x39000000,
  0xF9000000, 0x53000000, 0x37000000,
  0xFB000000, 0x49000000, 0x34000000,
};


GameColors::GameColors()
{

}


GameColors::~GameColors()
{

}


bool GameColors::Load()
{
  return true;
}


WORD* GameColors::GetBarrelBrownSpriteColors() const
{
  return m_sArrowSpriteColors.elem;
}

ULONG* GameColors::GetStrainSpreadColors() const
{
  return m_sStrainSpreadColors.elem;
}
