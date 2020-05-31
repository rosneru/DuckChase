
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "Arrow.h"

Arrow::Arrow(GameViewBase& gameView,
             const GameWorld& gameWorld,
             const ArrowResources& arrowResources,
             bool stealMouse)
  : EntityBase(gameWorld),
    m_GameView(gameView),
    m_Resources(arrowResources),
    m_Shape(m_GameView.ViewPort(), 
            arrowResources,
            stealMouse),
    m_Animator(m_Shape, arrowResources.AnimRightUpward()),
    m_ElapsedSinceLastAnimUpdate(0)
{
  m_Shape.SetVPortColorsForSprite(gameView.ViewPort(), 
                                  m_Resources.AnimRightUpward()->GetColors32());
  // Move to start position
  m_Shape.Move(180, 140);
  m_XSpeed_pps = -200;
}


Arrow::~Arrow()
{

}

void Arrow::Activate(int x, int y, long xSpeed_pps, long ySpeed_pps)
{
  m_Shape.Move(x, y);
  // m_XSpeed_pps = xSpeed_pps;
  // m_YSpeed_pps = ySpeed_pps;
  m_bIsAlive = true;
}


void Arrow::Update(unsigned long elapsed, unsigned long joyPortState)
{
  int dX = pps2Dist(m_XSpeed_pps, elapsed);
  int dY = pps2Dist(m_YSpeed_pps, elapsed);

  
  m_Shape.Move(m_Shape.Left() + dX, m_Shape.Top() + dY);


  if(m_Shape.X() < -m_Shape.Width())
  {
    m_Shape.Move(m_GameView.Width(), m_Shape.Y());
  }

  // Every some frames (or if the direction changed) switch the duck
  // image
  m_ElapsedSinceLastAnimUpdate += elapsed;
  if (m_ElapsedSinceLastAnimUpdate > 180)
  {
    m_ElapsedSinceLastAnimUpdate = 0;
    m_Animator.NextFrame();
  }
}
