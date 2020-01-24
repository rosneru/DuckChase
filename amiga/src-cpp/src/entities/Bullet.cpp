
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "Bullet.h"


Bullet::Bullet(IGameView& gameView, GameColors& colors, Hunter& hunter)
  : EntityBase(&m_Shape),
    m_GameView(gameView),
    m_GameColors(colors),
    m_Hunter(hunter),
    m_Shape(),
    m_BulletAnimSeq(16, 13, 2), // width, height, depth of each anim image
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
  // Loading all bullet anim sequences
  //
  const char* ppFiles[]  = {"gfx/bullet1_hires.raw",
                            "gfx/bullet3_hires.raw",
                            "gfx/bullet4_hires.raw",
                            "gfx/bullet5_hires.raw",
                            "gfx/bullet6_hires.raw",
                            "gfx/bullet7_hires.raw",
                            NULL};

  if(m_BulletAnimSeq.Load(ppFiles) == false)
  {
    m_pLastError = m_BulletAnimSeq.ErrorMsg();
    return false;
  }

  m_Shape.SetAnimSequence(&m_BulletAnimSeq);

  // We want the bullet sprite to replace the mouse pointer. So we set
  // the bullet sprite number to 0, which is the mouse pointers sprite
  // number. See AABoing source from Aminet
  int usedSpriteNumber = 0;
  m_Shape.Get()->es_SimpleSprite.num = usedSpriteNumber;

  //
  // Set the colors for the sprite
  //

  ULONG* colorsBulletSprite = m_GameColors.GetRGB32ArrowSprite();


  // Which 3 pens to set depends on the used sprite number 
  size_t startPen = 16 + ((usedSpriteNumber & 0x06) << 1);

  // But the first of the 4 sprite pens is always unused
  startPen++;

  size_t numCols = 3;
  size_t iColArray = 0;
  for(size_t iPen = startPen; iPen < (startPen + numCols); iPen++)
  {
    iColArray += numCols;
    int r = colorsBulletSprite[iColArray];
    int g = colorsBulletSprite[iColArray + 1];
    int b = colorsBulletSprite[iColArray + 2];
    SetRGB32(m_GameView.ViewPort(), iPen, r, g, b);
  }

  // Sprite must have a ViewPort to be displayed
  m_Shape.SetViewPort(m_GameView.ViewPort());

  // Move sprite at desiresd start position
  m_Shape.Move(300, 244);

  // Though for the beginning set it invisible
  m_Shape.SetInvisible();

  return true;
}


void Bullet::Update(unsigned long elapsed, unsigned long joyPortState)
{
  //
  // When invisible only check if fire button is pressed
  //
  if(m_Shape.IsVisible() == false)
  {
    if((joyPortState & JPF_BUTTON_RED) != 0)
    {
      //
      // Arming the bullet
      //
      m_Shape.Move(m_Hunter.XPos(), m_Hunter.YPos());
      m_Shape.SetVisible();

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

  if(m_Shape.YPos() + dY < 0)
  {
    m_Shape.SetInvisible();
    m_XSpeed_pps = 0;
    m_YSpeed_pps = 0;
  }
  else if(m_Shape.XPos() + dX < 1)
  {
    m_Shape.Move(640, m_Shape.YPos() + dY);
  }
  else if(m_Shape.XPos() + dX > 640)
  {
    m_Shape.Move(0, m_Shape.YPos() + dY);
  }
  else
  {
    m_Shape.Move(m_Shape.XPos() + dX, m_Shape.YPos() + dY);
  }


  //
  // Change the bullet sprite image every 10 frames
  //
  if(m_AnimFrameCnt % 10 == 0)
  {
    m_Shape.NextImage();
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
