
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "Duck.h"

Duck::Duck(GameViewBase& gameView,
           const GameWorld& gameWorld,
           const DuckResources& duckResources)
  : EntityBase(gameWorld),
    m_GameView(gameView),
    m_Resources(duckResources),
    m_Shape(m_GameView.RastPort(), 
            m_GameView.Depth(),
            duckResources),
    m_Animator(m_Shape, duckResources.AnimFlyLeft()),
    m_ElapsedSinceLastAnimUpdate(0)
{
  // Move to start position
  m_Shape.Move(800, 40);
  m_XSpeed_pps = -200;
}


Duck::~Duck()
{

}

void Duck::Activate(int x, int y, long xSpeed_pps, long ySpeed_pps)
{
  m_Shape.Move(x, y);
  // m_XSpeed_pps = xSpeed_pps;
  // m_YSpeed_pps = ySpeed_pps;
  m_bIsAlive = true;
}


void Duck::Update(unsigned long elapsed, unsigned long joyPortState)
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
