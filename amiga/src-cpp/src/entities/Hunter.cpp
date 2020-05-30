
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "GameTile.h"
#include "Hunter.h"

Hunter::Hunter(GameViewBase& gameView, 
                 const GameWorld& gameWorld, 
                 const HunterResources& jumpmanResources,
                 bool& isArrowLaunching, 
                 bool& isArrowLaunchDone)
  : EntityBase(gameWorld),
    m_GameView(gameView),
    m_Resources(jumpmanResources),
    m_Shape(gameView.RastPort(), 
            gameView.Depth(),
            jumpmanResources),
    m_Animator(m_Shape, jumpmanResources.AnimRightRun()),
    m_ElapsedSinceLastAnimUpdate(0),
    m_IsArrowLaunched(isArrowLaunchDone),
    m_IsLaunchingArrow(isArrowLaunching),
    m_IsRunning(false),
    m_LastDirection(JPF_JOY_RIGHT),
    m_UpClimbingPixelsLeft(0)
{
  m_Shape.Move(20, 216);
}


Hunter::~Hunter()
{

}

void Hunter::Activate(int x, int y, long xSpeed_pps, long ySpeed_pps)
{
  m_Shape.Move(x, y);
  // m_XSpeed_pps = xSpeed_pps;
  // m_YSpeed_pps = ySpeed_pps;
  m_bIsAlive = true;
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
    if(launchArrow())
      isMoving = true;
  }
  else
  {
    // resetJumpmanActions();
  }

  // Every some frames (or if the direction changed) switch the hunter
  // image
  m_ElapsedSinceLastAnimUpdate += elapsed;
  if (m_ElapsedSinceLastAnimUpdate > 80)
  {
    m_ElapsedSinceLastAnimUpdate = 0;
    if(isMoving)
    {
      // m_Shape.NextImage();
      m_Animator.NextFrame();
    }
    else
    {
      // m_Animator.FirstFrame();
      m_Animator.FirstFrame();
    }
  }
}

bool Hunter::runLeft(unsigned long elapsed)
{

}


bool Hunter::runRight(unsigned long elapsed)
{
  m_XSpeed_pps = 200;

  // Check if direction has changed
  if(m_LastDirection != JPF_JOY_RIGHT)
  {
    m_ElapsedSinceLastAnimUpdate = 0;
    m_LastDirection = JPF_JOY_RIGHT;

    m_Animator.SetAnimSeq(m_Resources.AnimRightRun());
    m_Animator.FirstFrame();
  }

  int dX = pps2Dist(m_XSpeed_pps, elapsed);

  if(m_Shape.X() + dX > m_GameView.Width() + m_Shape.Width())
  {
    m_Shape.Move(-m_Shape.Width(), m_Shape.Y());
  }
  else
  {
    m_Shape.Move(m_Shape.X() + dX, m_Shape.Y());
  }


  return true;
}


bool Hunter::launchArrow()
{


  return true;
}
