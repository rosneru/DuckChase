
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "GameTile.h"
#include "Hunter.h"

Hunter::Hunter(GameViewBase& gameView, 
                 const GameWorld& gameWorld, 
                 const HunterResources& jumpmanResources,
                 Arrow& arrow,
                 bool& isArrowFlightPrepared)
  : EntityBase(gameWorld),
    m_GameView(gameView),
    m_Resources(jumpmanResources),
    m_Shape(gameView.RastPort(), 
            gameView.Depth(),
            jumpmanResources),
    m_Animator(m_Shape, jumpmanResources.AnimRightRun()),
    m_ElapsedSinceLastAnimUpdate(0),
    m_Arrow(arrow),
    m_IsArrowFlightPrepared(isArrowFlightPrepared),
    m_IsRunning(false),
    m_LastDirection(JPF_JOY_RIGHT),
    m_UpClimbingPixelsLeft(0)
{
  m_Shape.Move(20, 196);
}


Hunter::~Hunter()
{

}


ShapeBase& Hunter::Shape()
{
  return m_Shape;
}


void Hunter::Activate(int x, int y, long xSpeed, long ySpeed)
{
  m_Shape.Move(x, y);
  m_Shape.SetVisible();
  m_IsAlive = true;
}


void Hunter::Deactivate()
{
  m_IsAlive = false;
  m_Shape.SetInvisible();
}


void Hunter::Update(unsigned long elapsed, unsigned long portState)
{
  bool isMoving = false;

  // Left/right movement
  if((portState & JPF_JOY_RIGHT) != 0)
  {
    runRight(elapsed);
    isMoving = true;
  }
  else if((portState & JPF_JOY_LEFT) != 0)
  {
    runLeft(elapsed);
    isMoving = true;
  }
  else if((portState & JPF_BTN2) != 0)
  {
    prepareArrowLaunch();
  }
  else
  {
    resetHunterActions();
  }

  // Every some frames (or if the direction changed) switch the hunter
  // image
  m_ElapsedSinceLastAnimUpdate += elapsed;
  if (m_ElapsedSinceLastAnimUpdate > 140)
  {
    m_ElapsedSinceLastAnimUpdate = 0;
    if(isMoving)
    {
      m_Animator.NextFrame();
    }
    else
    {
      m_Animator.FirstFrame();
    }
  }
}

bool Hunter::runLeft(unsigned long elapsed)
{
  m_XSpeed = -140;

    // Check if direction has changed
  if(m_LastDirection != JPF_JOY_LEFT)
  {
    m_ElapsedSinceLastAnimUpdate = 0;
    m_LastDirection = JPF_JOY_LEFT;

    m_Animator.SetAnimSeq(m_Resources.AnimLeftRun());
    m_Animator.FirstFrame();
  }

  int dX = pps2Dist(m_XSpeed, elapsed);

  if(m_Shape.X() + dX < -m_Shape.Width())
  {
    m_Shape.Move(m_GameView.Width() + m_Shape.Width(), m_Shape.Y());
  }
  else
  {
    m_Shape.Move(m_Shape.X() + dX, m_Shape.Y());
  }

  return true;
}


bool Hunter::runRight(unsigned long elapsed)
{
  m_XSpeed = 140;

  // Check if direction has changed
  if(m_LastDirection != JPF_JOY_RIGHT)
  {
    m_ElapsedSinceLastAnimUpdate = 0;
    m_LastDirection = JPF_JOY_RIGHT;

    m_Animator.SetAnimSeq(m_Resources.AnimRightRun());
    m_Animator.FirstFrame();
  }

  int dX = pps2Dist(m_XSpeed, elapsed);

  if(m_Shape.X() + dX > m_GameView.Width())
  {
    m_Shape.Move(-m_Shape.Width(), m_Shape.Y());
  }
  else
  {
    m_Shape.Move(m_Shape.X() + dX, m_Shape.Y());
  }

  return true;
}

void Hunter::prepareArrowLaunch()
{
  if(m_IsArrowFlightPrepared || m_Arrow.IsAlive())
  {
    return;
  }

  m_IsArrowFlightPrepared = true;
  m_IsRunning = false;

  if(m_LastDirection == JPF_RIGHT)
  {
    m_Animator.SetAnimSeq(m_Resources.AnimRightShoot());
    m_Animator.FirstFrame();
  }
  else
  {
    m_Animator.SetAnimSeq(m_Resources.AnimLeftShoot());
    m_Animator.FirstFrame();
  }
}

void Hunter::resetHunterActions()
{
  if(!m_IsArrowFlightPrepared && !m_IsRunning)
  {
    return;
  }

  m_XSpeed = 0;

  if(m_LastDirection == JPF_RIGHT)
  {
    m_Animator.SetAnimSeq(m_Resources.AnimRightRun());
    m_Animator.FirstFrame();
  }
  else
  {
    m_Animator.SetAnimSeq(m_Resources.AnimLeftRun());
    m_Animator.FirstFrame();
  }
  
  if(m_IsArrowFlightPrepared)
  {
    m_IsArrowFlightPrepared = false;

    if(m_Arrow.IsAlive())
    {
      // Arrow is already launched
      return;
    }

    if(m_LastDirection == JPF_RIGHT)
    {
      // Start the arrow in right direction. Use speed fieds only to
      // give the direction to the arrow.
      m_Arrow.Activate(m_Shape.X(), m_Shape.Y(), 1, 0);
    }
    else
    {
      // Start the arrow in left direction. Use speed fieds only to
      // give the direction to the arrow.
      m_Arrow.Activate(m_Shape.X(), m_Shape.Y(), -1, 0);
    }

    return;
  }

  if(m_IsRunning)
  {
    m_IsRunning = false;
    return;
  }
}