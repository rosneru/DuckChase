
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
    m_Animator(m_Shape, jumpmanResources.AnimRunRight()),
    m_ElapsedSinceLastAnimUpdate(0),
    m_IsArrowLaunched(isArrowLaunchDone),
    m_IsLaunchingArrow(isArrowLaunching),
    m_IsRunning(false),
    m_LastDirection(JPF_JOY_RIGHT),
    m_UpClimbingPixelsLeft(0)
{
  //
  // Initialize postion of the shape and add it to the scene
  //
  m_WidthHalf = m_Shape.Width() / 2;
  m_WidthQuarter = m_Shape.Width() / 4;
  m_XSpeed_pps = 30;
  m_YSpeed_pps = 30;

  m_Shape.SetHotspot(m_WidthHalf, m_Shape.Height());

  // Move Jumpman to its start position
  const GameTile* pStartTile = m_GameWorld.Tile(100, 210);
  m_Shape.Move(pStartTile->Left(), pStartTile->PlatformTop() - 1);
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
  else if((portState & JPF_JOY_UP) != 0)
  {
    if(climbLadderUp(elapsed))
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
  int shapeOverWidthCorrection = 0;

  // Check if direction has changed
  if(m_LastDirection != JPF_JOY_LEFT)
  {
    if(m_LastDirection == JPF_JOY_RIGHT)
    {
      // As long as Jumpman does not have a hammer, only one half of the
      // shape width is used for the image. When running to left the
      // left half is unused. This factor takes this into account for
      // the first calculation of newX after Jumpans running direction
      // has changed.
      shapeOverWidthCorrection = -m_WidthHalf;
    }

    m_ElapsedSinceLastAnimUpdate = 0;
    m_LastDirection = JPF_JOY_LEFT;

    m_Shape.SetHotspot(m_Shape.Width(), m_Shape.Height());

    m_Animator.SetAnimSeq(m_Resources.AnimRunLeft());
    m_Animator.FirstFrame();
  }

  int dX = pps2Dist(-m_XSpeed_pps, elapsed);
  int newX = m_Shape.X() + dX + shapeOverWidthCorrection;

  // Getting tile of x-destination position. NOTE: -6 manually adjusted
  // beacuse of actual shape image appearance
  const GameTile* pTile = m_GameWorld.Tile(newX - 6, m_Shape.Y());
  if(pTile->IsBorder())
  {
    return false;
  }

  int newY = pTile->PlatformTop() - 1;
  m_Shape.Move(newX, newY);
  return true;
}


bool Hunter::runRight(unsigned long elapsed)
{
  int shapeOverWidthCorrection = 0;

  // Check if direction has changed
  if(m_LastDirection != JPF_JOY_RIGHT)
  {
    // As long as Jumpman does not have a hammer, only one half of the
    // shape width is used for the image. When running to right the
    // right half is unused. This factor takes this into account for
    // the first calculation of newX after Jumpans running direction
    // has changed.
    shapeOverWidthCorrection = m_WidthHalf;

    m_ElapsedSinceLastAnimUpdate = 0;
    m_LastDirection = JPF_JOY_RIGHT;

    m_Shape.SetHotspot(m_WidthHalf, m_Shape.Height());

    m_Animator.SetAnimSeq(m_Resources.AnimRunRight());
    m_Animator.FirstFrame();
  }

  int dX = pps2Dist(m_XSpeed_pps, elapsed);
  int newX = m_Shape.X() + dX + shapeOverWidthCorrection;

  // Getting tile of x-destination position. NOTE: -4 manually adjusted
  // beacuse of actual shape image appearance
  const GameTile* pTile = m_GameWorld.Tile(newX - 4, m_Shape.Y());
  if(pTile->IsBorder())
  {
    return false;
  }

  int newY = pTile->PlatformTop() - 1;
  m_Shape.Move(newX, newY);


  return true;
}


bool Hunter::climbLadderUp(unsigned long elapsed)
{
  if(m_LastDirection != JPF_JOY_UP)
  {
    // Direction has changed; initialize the number of pixels to climb
    if(m_UpClimbingPixelsLeft == 0)
    {
      if(m_GameWorld.CanNumPixelsLadderUp(m_Shape.X() - m_WidthQuarter, // x-center of Jumpman
                                          m_Shape.Y()))
      {
        m_UpClimbingPixelsLeft = m_GameWorld.PixToNextAbovePlatformTop(m_Shape.X(),
                                                                       m_Shape.Y());
      }
      else
      {
        return false;
      }
      
    }

    if(m_LastDirection == JPF_JOY_RIGHT)
    {
      // Jumpman before was running right and there the anim frame
      // images only use half of the shape width and are left-aligned.
      // Now, the climbing images frames also use half of the shape
      // width but are right-alinged. This factor takes this into
      // account for the first calculation of newX when Jumpan starts to
      // climb up a ladder.
      // shapeOverWidthCorrection = m_WidthHalf;
      
      m_Shape.Move(m_Shape.X() - m_WidthHalf, m_Shape.Y());
      m_Shape.SetHotspot(m_Shape.Width(), m_Shape.Height());
    }
 
    m_ElapsedSinceLastAnimUpdate = 0;
    m_LastDirection = JPF_JOY_UP;

    m_Animator.SetAnimSeq(m_Resources.AnimRunClimb1());
    m_Animator.FirstFrame();
  }

  if(m_UpClimbingPixelsLeft < 1)
  {
    return false;
  }
  
  int dY = pps2Dist(m_YSpeed_pps, elapsed);
  if(dY > m_UpClimbingPixelsLeft)
  {
    m_UpClimbingPixelsLeft = dY;
  }

  int newY = m_Shape.Y() - dY;
  m_Shape.Move(m_Shape.X(), newY);

  m_UpClimbingPixelsLeft -= dY;

  return true;
}
