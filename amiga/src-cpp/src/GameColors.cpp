#include "GameColors.h"


/**
 * The default color set for the view
 */
GameColors::ViewColors GameColors::m_sViewColors =
{
  0x00080000, // 0x0008 - Load 8 colors, starting from 0x000
  0xa0a0a0a0, 0xa0a0a0a0, 0xa0a0a0a0,
  0x00000000, 0x00000000, 0x00000000,
  0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0,
  0x60606060, 0x80808080, 0xb0b0b0b0,
  0x50505050, 0xa0a0a0a0, 0x30303030,
  0xe0e0e0e0, 0xb0b0b0b0, 0x00000000,
  0xb0b0b0b0, 0x50505050, 0x20202020,
  0xf0f0f0f0, 0x80808080, 0x00000000,
  0x00000000  // Termination
};

GameColors::ArrowSpriteColors GameColors::m_sArrowSpriteColors =
{
  0xE5E5E5E5, 0x14141414, 0x1D1D1D1D,
  0xC2C2C2C2, 0x5A5A5A5A, 0x20202020,
  0x2E2E2E2E, 0x14141414, 0x9090909
};


GameColors::GameColors()
  : m_RGB32View(m_sViewColors)
{

}


GameColors::~GameColors()
{

}


bool GameColors::Load()
{
  return true;
}

ULONG* GameColors::GetRGB32View()
{
  return m_RGB32View.elem;
}

ULONG* GameColors::GetRGB32ArrowSprite()
{
  return m_RGB32ArrowSprite.elem;
}
