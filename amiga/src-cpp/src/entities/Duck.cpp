
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
  m_XSpeed = -200;
}


Duck::~Duck()
{

}


ShapeBase& Duck::Shape()
{
  return m_Shape;
}


void Duck::Activate(int x, int y, long xSpeed, long ySpeed)
{
  m_Shape.Move(x, y);
  m_bIsAlive = true;
}


void Duck::Deactivate()
{
  m_bIsAlive = false;
  m_Shape.SetInvisible();
}


void Duck::Update(unsigned long elapsed, unsigned long joyPortState)
{
  int dX = pps2Dist(m_XSpeed, elapsed);
  int dY = pps2Dist(m_YSpeed, elapsed);

  
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
