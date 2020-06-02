
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "Arrow.h"

Arrow::Arrow(GameViewBase& gameView,
             const GameWorld& gameWorld,
             const ArrowResources& arrowResources,
             size_t& strain,
             bool stealMouse)
  : EntityBase(gameWorld),
    m_GameView(gameView),
    m_Resources(arrowResources),
    m_Strain(strain),
    m_Shape(m_GameView.ViewPort(), 
            arrowResources,
            stealMouse),
    m_Animator(m_Shape, arrowResources.AnimRightUpward()),
    m_ElapsedSinceLastAnimUpdate(0)
{
  m_Shape.SetVPortColorsForSprite(gameView.ViewPort(), 
                                  m_Resources.AnimRightUpward()->GetColors32());
  // Move to start position
  m_Shape.Move(-m_Shape.Width(), -m_Shape.Height());
  m_XSpeed_pps = -200;
}


Arrow::~Arrow()
{

}

void Arrow::Activate(int x, int y, long xSpeed_pps, long ySpeed_pps)
{
  m_Shape.Move(x, y);

  if(xSpeed_pps >= 0)
  {
    m_Animator.SetAnimSeq(m_Resources.AnimRightUpward());
    m_XSpeed_pps = 3 * m_Strain;
  }
  else
  {
    m_Animator.SetAnimSeq(m_Resources.AnimLeftUpward());
    m_XSpeed_pps = -3 * m_Strain;
  }
  

  m_bIsAlive = true;
}

void Arrow::Deactivate()
{
  m_bIsAlive = false;
  m_Shape.SetInvisible();
}

void Arrow::Update(unsigned long elapsed, unsigned long joyPortState)
{
  if(!m_bIsAlive)
  {
    return;
  }

  if(m_Shape.IsGone())
  {
    m_Shape.SetInvisible();
    m_bIsAlive = false;  
    return;
  }

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
