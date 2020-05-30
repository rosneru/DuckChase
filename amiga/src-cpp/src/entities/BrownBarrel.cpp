
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "BrownBarrel.h"

BrownBarrel::BrownBarrel(GameViewBase& gameView,
                         const GameWorld& gameWorld,
                         const BrownBarrelResources& brownBarrelResources)
  : EntityBase(gameWorld),
    m_GameView(gameView),
    m_Resources(brownBarrelResources),
    m_Shape(m_GameView.RastPort(), 
            m_GameView.Depth(),
            brownBarrelResources),
    m_Animator(m_Shape, brownBarrelResources.AnimHorizontally()),
    m_AnimFrameCnt(1)
{
  m_Action = EntityBase::MoveVertically;

  // At start move the barrel out of the screen
  m_Shape.Move(m_Shape.Width(), -m_Shape.Height());
}


BrownBarrel::~BrownBarrel()
{

}

void BrownBarrel::Activate(int x, int y, long xSpeed_pps, long ySpeed_pps)
{
  m_Shape.Move(x, y);
  m_XSpeed_pps = xSpeed_pps;
  m_YSpeed_pps = ySpeed_pps;
  m_bIsAlive = true;
}


void BrownBarrel::Update(unsigned long elapsed, unsigned long joyPortState)
{
  if(m_Shape.IsVisible() && m_Shape.IsGone())
  {
    m_bIsAlive = false;
    return;
  }

  int dX = pps2Dist(m_XSpeed_pps, elapsed);
  int dY = pps2Dist(m_YSpeed_pps, elapsed);

  if(m_Action == EntityBase::MoveHorizontally)
  {
    // Check the next two pixels below if there's yet a platform
    // (marked by color 8)
    int yDistNextPlate = 
      m_Shape.BottomToPlatformDistance(m_GameView.BackgroundPicture(), 
                                       2, 
                                       8,
                                       ShapeBase::BDMM_Middle);

    dY = 0;
    if(yDistNextPlate == -1)
    {
      m_Action = EntityBase::MoveVertically;
    }
    else
    {
      dY = yDistNextPlate;
    }
  }
  else if(m_Action == EntityBase::MoveVertically)
  {
    // Checking the distance to next platform (color 8) below
    int yDistNextPlate = 
      m_Shape.BottomToPlatformDistance(m_GameView.BackgroundPicture(), 
                                       dY, 
                                       8,
                                       ShapeBase::BDMM_Middle);

    if(yDistNextPlate > -1)
    {
      // TODO Add m_Shape.GetPlatformSLope() and determine MoveLeft/MoveRight from there
      //m_Action = EntityBase::MoveLeft;
      dY = yDistNextPlate;
    }
  }

  if(dX >= 0)
  {
    // Check collission on right border and
    int diffToRightBorder = m_GameView.BorderRight() - m_Shape.Width() - m_Shape.Left();

    if(dX > diffToRightBorder)
    {
      dX = diffToRightBorder;
    }

    if(dX == 0)
    {
      // Can't move to right anymore, then move to left
      m_XSpeed_pps = -m_XSpeed_pps;
      dX = pps2Dist(m_XSpeed_pps, elapsed);

      // Set the barrel roll animation to be played backwards
      m_Animator.SetAnimateBackward(true);
    }
  }
  else
  {
    int diffToLeftBorder = m_Shape.Left() - m_GameView.BorderLeft();

    if((-dX) > diffToLeftBorder)
    {
      dX = -diffToLeftBorder;
    }

    if(dX == 0)
    {
      // Can't move to left anymore, then move to right
      m_XSpeed_pps = -m_XSpeed_pps;
      dX = pps2Dist(m_XSpeed_pps, elapsed);

      // Set the barrel roll animation to be played forwards
      m_Animator.SetAnimateBackward(false);
    }
  }
  
  
  m_Shape.Move(m_Shape.Left() + dX, m_Shape.Top() + dY);

  //
  // Change the barrel sprite image every 10 frames
  //
  if(m_AnimFrameCnt % 6 == 0)
  {
    m_Animator.NextFrame();
    m_AnimFrameCnt = 0;
  }

  m_AnimFrameCnt++;
  
}
