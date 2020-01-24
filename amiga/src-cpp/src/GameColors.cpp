#include "GameColors.h"


/**
 * The default color set
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

