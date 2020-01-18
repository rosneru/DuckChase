
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "Bullet.h"


Bullet::Bullet(IGameView& gameView, Hunter& hunter)
  : m_GameView(gameView),
    m_Hunter(hunter),
    m_Sprite(16, 13),    // TODO Remove constants / find better solution
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
  if(m_Sprite.AddRawImage("gfx/bullet1_hires.raw") == false)
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
  if(m_Sprite.AddRawImage("gfx/bullet3_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #3 (gfx/bullet3_hires.raw).\n";
    return false;
  }

  if(m_Sprite.AddRawImage("gfx/bullet4_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #4 (gfx/bullet4_hires.raw).\n";
    return false;
  }

  if(m_Sprite.AddRawImage("gfx/bullet5_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #5 (gfx/bullet5_hires.raw).\n";
    return false;
  }

  if(m_Sprite.AddRawImage("gfx/bullet6_hires.raw") == false)
  {
    m_pLastError = "Couldn't load bullet image #6 (gfx/bullet6_hires.raw).\n";
    return false;
  }

  if(m_Sprite.AddRawImage("gfx/bullet7_hires.raw") == false)
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

  // Which 4 pens to set depends on the sprite number we got. But we'll
  // manually set the number to 0 because: 
  //   A relatively safe way to replace the mouse pointer (sprite 0) 
  //   with the arrow sprite (See AABoing source from Aminet) is to 
  //   simply set it to 0. This is the number the mouse uses otherwise.
  int spriteNumberInUse = 0;
  m_Sprite.Get()->es_SimpleSprite.num = spriteNumberInUse;

  int spriteColRegStart = 16 + ((spriteNumberInUse & 0x06) << 1);
  for(int i = spriteColRegStart; i < (spriteColRegStart + 4); i++)
  {
    int r = colorsBulletSprite[i - spriteColRegStart][0];
    int g = colorsBulletSprite[i - spriteColRegStart][1];
    int b = colorsBulletSprite[i - spriteColRegStart][2];
    m_GameView.SetColor32(i, r, g, b);
  }

  // Sprite must have a ViewPort to be displayed
  m_Sprite.SetViewPort(m_GameView.ViewPort());

  // Move sprite at desiresd start position
  m_Sprite.Move(300, 244);

  // Though for the beginning set it invisible
  m_Sprite.SetInvisible();

  return true;
}


void Bullet::Update(unsigned long elapsed, unsigned long joyPortState)
{
  //
  // When invisible only check if fire button is pressed
  //
  if(m_Sprite.IsVisible() == false)
  {
    if((joyPortState & JPF_BUTTON_RED) != 0)
    {
      //
      // Arming the bullet
      //
      m_Sprite.Move(m_Hunter.XPos(), m_Hunter.YPos());
      m_Sprite.SetVisible();

      m_XSpeed_pps = m_Hunter.XSpeed_pps();
      m_YSpeed_pps = -150;
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

  if(m_Sprite.YPos() + dY < 0)
  {
    m_Sprite.SetInvisible();
    m_XSpeed_pps = 0;
    m_YSpeed_pps = 0;
  }
  else if(m_Sprite.XPos() + dX < 1)
  {
    m_Sprite.Move(640, m_Sprite.YPos() + dY);
  }
  else if(m_Sprite.XPos() + dX > 640)
  {
    m_Sprite.Move(0, m_Sprite.YPos() + dY);
  }
  else
  {
    m_Sprite.Move(m_Sprite.XPos() + dX, m_Sprite.YPos() + dY);
  }


  //
  // Change the bullet sprite image every 10 frames
  //
  if(m_AnimFrameCnt % 10 == 0)
  {
    m_Sprite.NextImage();
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


int Bullet::pps2Dist(int pps, long elapsed_ms)
{
  return pps * elapsed_ms / 1000;
}
