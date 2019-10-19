
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "Bullet.h"


Bullet::Bullet(IGameView& gameView, Hunter& hunter)
  : HwSprite(16, 13), // TODO find better solution
    m_GameView(gameView),
    m_Hunter(hunter),
    m_pLastError(NULL),
    m_AnimFrameCnt(1),
    m_XSpeed_pps(0),
    m_YSpeed_pps(0)
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
  if(AddRawImage("gfx/bullet1_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #1 (gfx/bullet1_hires.raw).\n";
    return false;
  }
/*
  if(AddRawImage("gfx/bullet2_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #2 (gfx/bullet2_hires.raw).\n";
    return false;
  }
*/
  if(AddRawImage("gfx/bullet3_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #3 (gfx/bullet3_hires.raw).\n";
    return false;
  }

  if(AddRawImage("gfx/bullet4_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #4 (gfx/bullet4_hires.raw).\n";
    return false;
  }

  if(AddRawImage("gfx/bullet5_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #5 (gfx/bullet5_hires.raw).\n";
    return false;
  }

  if(AddRawImage("gfx/bullet6_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #6 (gfx/bullet6_hires.raw).\n";
    return false;
  }

  if(AddRawImage("gfx/bullet7_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #7 (gfx/bullet7_hires.raw).\n";
    return false;
  }
/*
  if(AddRawImage("gfx/bullet8_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #8 (gfx/bullet8_hires.raw).\n";
    return false;
  }
*/


  //
  // Set the colors for the sprite
  //

  ULONG colorsBulletSprite[4][3] =
  {
    {0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA},
    {0xE5E5E5E5, 0x14141414, 0x1D1D1D1D},
    {0xC2C2C2C2, 0x5A5A5A5A, 0x20202020},
    {0x2E2E2E2E, 0x14141414, 0x9090909},
  };

  // Which 4 pens  to set depends on the sprite number we got
  int spriteNum = SpriteNumber();

  int spriteColRegStart = 16 + ((spriteNum & 0x06) << 1);
  for(int i = spriteColRegStart; i < (spriteColRegStart + 4); i++)
  {
    int r = colorsBulletSprite[i - spriteColRegStart][0];
    int g = colorsBulletSprite[i - spriteColRegStart][1];
    int b = colorsBulletSprite[i - spriteColRegStart][2];
    m_GameView.SetColor32(i, r, g, b);
  }

  // Sprite must have a ViewPort to be displayed
  SetViewPort(m_GameView.ViewPort());

  // Move sprite at desiresd start position
  Move(300, 244);

  // Though for the beginning set it invisible
  SetInvisible();

  return true;
}


void Bullet::Update(unsigned long elapsed, unsigned long joyPortState)
{
  //
  // When invisible only check if fire button is pressed
  //
  if(IsVisible() == false)
  {
    if((joyPortState & JP_TYPE_MASK) == JP_TYPE_JOYSTK)
    {
      if((joyPortState & JPF_BUTTON_RED) != 0)
      {
        //
        // Arming the bullet
        //
        Move(m_Hunter.XPos(), m_Hunter.YPos());
        SetVisible();

        m_XSpeed_pps = m_Hunter.XSpeed_pps();
        m_YSpeed_pps = -150;
      }
    }

    return;
  }

  //
  // Move bullet sprite to new position
  //
  m_XSpeed_pps *= 1.01;
  m_YSpeed_pps *= 1.01;

  int dX = pps2Dist(m_XSpeed_pps, elapsed);
  int dY = pps2Dist(m_YSpeed_pps, elapsed);

  if(YPos() + dY < 0)
  {
    SetInvisible();
    m_XSpeed_pps = 0;
    m_YSpeed_pps = 0;
  }
  else if(XPos() + dX < 1)
  {
    Move(640, YPos() + dY);
  }
  else if(XPos() + dX > 640)
  {
    Move(0, YPos() + dY);
  }
  else
  {
    Move(XPos() + dX, YPos() + dY);
  }


  //
  // Change the bullet sprite image every 10 frames
  //
  if(m_AnimFrameCnt % 10 == 0)
  {
    NextImage();
    m_AnimFrameCnt = 0;
  }

  m_AnimFrameCnt++;
}


const char* Bullet::LastError() const
{
  return m_pLastError;
}

int Bullet::XSpeed_pps()
{
  return m_XSpeed_pps;
}

int Bullet::YSpeed_pps()
{
  return m_YSpeed_pps;
}
