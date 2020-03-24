
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "Arrow.h"


Arrow::Arrow(IGameView& gameView, GameColors& colors, Hunter& hunter)
  : EntityBase(&m_Shape),
    m_GameView(gameView),
    m_GameColors(colors),
    m_Hunter(hunter),
    m_Shape(),
    m_ArrowAnimSeq(16, 8, 2), // width, height, depth of each anim image
    m_pLastError(NULL),
    m_AnimFrameCnt(1),
    m_XSpeed_pps(0),
    m_YSpeed_pps(0)
{

}


Arrow::~Arrow()
{

}


bool Arrow::Init()
{

  //
  // Loading all bullet anim sequences
  //
  const char* ppFiles[]  = {"gfx/arrow_right1.raw",
                            NULL};

  if(m_ArrowAnimSeq.Load(ppFiles) == false)
  {
    m_pLastError = m_ArrowAnimSeq.ErrorMsg();
    return false;
  }

  m_Shape.SetAnimSequence(&m_ArrowAnimSeq);

  // We want the bullet sprite to replace the mouse pointer
  m_Shape.UseMouseSprite();

  // Set the colors for the sprite in view port
  m_Shape.SetVPortColorsForSprite(m_GameView.ViewPort(),
                                  m_GameColors.GetArrowSpriteColors());

  // Sprite must have a ViewPort to be displayed
  m_Shape.SetViewPort(m_GameView.ViewPort());

  // Move sprite at desired start position
  m_Shape.Move(300, 244);

  // Though for the beginning set it invisible
  m_Shape.SetInvisible();

  return true;
}


void Arrow::Update(unsigned long elapsed, unsigned long joyPortState)
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


const char* Arrow::LastError() const
{
  return m_pLastError;
}

int Arrow::XSpeed_pps()
{
  return m_XSpeed_pps;
}

int Arrow::YSpeed_pps()
{
  return m_YSpeed_pps;
}