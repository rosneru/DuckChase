
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "Bullet.h"

Bullet::Bullet(IGameView& gameView)
  : HwSprite(16, 13), // TODO find better solution
    m_GameView(gameView),
    m_pSprite(NULL),
    m_pLastError(NULL),
    m_AnimFrameCnt(1) // TODO CHECK Why not 0?
{

}

Bullet::~Bullet()
{

}

bool Bullet::Init()
{
  //
  // Loading all the hunter images
  //
  if(AddRawImage("/gfx/bullet1_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #1 (/gfx/bullet1_hires.raw).\n";
    return false;
  }

  if(AddRawImage("/gfx/bullet2_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #2 (/gfx/bullet2_hires.raw).\n";
    return false;
  }

  if(AddRawImage("/gfx/bullet3_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #3 (/gfx/bullet3_hires.raw).\n";
    return false;
  }

  if(AddRawImage("/gfx/bullet4_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #4 (/gfx/bullet4_hires.raw).\n";
    return false;
  }

  if(AddRawImage("/gfx/bullet5_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #5 (/gfx/bullet5_hires.raw).\n";
    return false;
  }

  if(AddRawImage("/gfx/bullet6_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #6 (/gfx/bullet6_hires.raw).\n";
    return false;
  }

  if(AddRawImage("/gfx/bullet7_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #7 (/gfx/bullet7_hires.raw).\n";
    return false;
  }

  if(AddRawImage("/gfx/bullet8_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #8 (/gfx/bullet8_hires.raw).\n";
    return false;
  }

  //
  // Trying to ackquire the bob
  //
  m_pSprite = Get();

  if(m_pSprite == NULL)
  {
    m_pLastError = "Couldn't acquire the bullet sprite.\n";
    return false;
  }

  // Set the colors for the sprite
  ULONG colorsBulletSprite[4][3] =
  {
    {0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA},
    {0xE5E5E5E5, 0x14141414, 0x1D1D1D1D},
    {0xC2C2C2C2, 0x5A5A5A5A, 0x20202020},
    {0x2E2E2E2E, 0x14141414, 0x9090909},
  };

  int spriteNum = SpriteNumber();
  int spriteColRegStart = 16 + ((spriteNum & 0x06) << 1);
  for(int i = spriteColRegStart; i < (spriteColRegStart + 4); i++)
  {
    int r = colorsBulletSprite[i - spriteColRegStart][0];
    int g = colorsBulletSprite[i - spriteColRegStart][1];
    int b = colorsBulletSprite[i - spriteColRegStart][2];
    SetRGB32(m_GameView.ViewPort(), i, r, g, b);
  }

  // Move sprite at desiresd start position
  MoveSprite(m_GameView.ViewPort(), 
             (struct SimpleSprite*)m_pSprite,
             300, 244);
}

void Bullet::Update(unsigned long elapsed, unsigned long joyPortState)
{
  if(m_AnimFrameCnt % 10 == 0)
  {
    // Change the bullet sprite image every frame
    struct ExtSprite* pCurrSpriteImg = m_pSprite;
    
    NextImage();
    m_pSprite = Get();
    
    ChangeExtSprite(m_GameView.ViewPort(),
                    pCurrSpriteImg,
                    m_pSprite,
                    TAG_END);
    
    m_AnimFrameCnt = 0;
  }

  m_AnimFrameCnt++;
}
