
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "Hunter.h"

Hunter::Hunter(IGameView& gameView, bool& isArrowArmed)
  : EntityBase(&m_Shape),
    m_GameView(gameView),
    m_Shape(gameView.Depth()),
    m_HunterRightAnim(25, 27, 4), // width, height, depth of each anim image
    m_HunterRightShootAnim(25, 27, 4),
    m_HunterLeftAnim(25, 27, 4),
    m_HunterLeftShootAnim(25, 27, 4),
    m_pLastError(NULL),
    m_AnimFrameCnt(1),  // TODO CHECK Why not 0?
    m_XSpeed_pps(0),
    m_YSpeed_pps(0),
    m_IsArrowArmed(isArrowArmed),
    m_IsRunning(false),
    m_LastDirection(JPF_JOY_RIGHT)
{

}

Hunter::~Hunter()
{

}

bool Hunter::Init()
{
  //
  // Loading all hunter anim sequences
  //
  const char* pHunterRightFiles[]  = {"gfx/hunter_right1.raw",
                                      "gfx/hunter_right2.raw",
                                      NULL};

  const char* pHunterRightShootFiles[]  = {"gfx/hunter_right_shoot.raw",
                                           NULL};

  const char* pHunterLeftFiles[]  = {"gfx/hunter_left1.raw",
                                     "gfx/hunter_left2.raw",
                                     NULL};

  const char* pHunterLeftShootFiles[]  = {"gfx/hunter_Left_shoot.raw",
                                          NULL};

  if(m_HunterRightAnim.Load(pHunterRightFiles) == false)
  {
    m_pLastError = m_HunterRightAnim.ErrorMsg();
    return false;
  }

  if(m_HunterRightShootAnim.Load(pHunterRightShootFiles) == false)
  {
    m_pLastError = m_HunterRightShootAnim.ErrorMsg();
    return false;
  }

  if(m_HunterLeftAnim.Load(pHunterLeftFiles) == false)
  {
    m_pLastError = m_HunterLeftAnim.ErrorMsg();
    return false;
  }

  if(m_HunterLeftShootAnim.Load(pHunterLeftShootFiles) == false)
  {
    m_pLastError = m_HunterLeftShootAnim.ErrorMsg();
    return false;
  }

  // Starting with the right anim
  m_Shape.SetAnimSequence(&m_HunterRightAnim);

  //
  // Initialize postion of the duck bob and add it to the scene
  //
  m_Shape.SetRastPort(m_GameView.RastPort());
  m_Shape.Move(20, 216);

  return true;
}

void Hunter::Update(unsigned long elapsed, unsigned long portState)
{
  // Left/right movement
  if((portState & JPF_JOY_RIGHT) != 0)
  {
    runRight(elapsed);
  }
  else if((portState & JPF_JOY_LEFT) != 0)
  {
    m_XSpeed_pps = -200;
    int dX = pps2Dist(m_XSpeed_pps, elapsed);

    if(m_Shape.XPos() + dX < -m_Shape.Width())
    {
      m_Shape.Move(640 + m_Shape.Width(), m_Shape.YPos());
    }
    else
    {
      m_Shape.Move(m_Shape.XPos() + dX, m_Shape.YPos());
    }
  }
  else
  {
    m_XSpeed_pps = 0;
  }
}


const char* Hunter::LastError() const
{
  return m_pLastError;
}

int Hunter::XSpeed_pps()
{
  return m_XSpeed_pps;
}

int Hunter::YSpeed_pps()
{
  return m_YSpeed_pps;
}


void Hunter::runLeft(unsigned long elapsed)
{

}


void Hunter::runRight(unsigned long elapsed)
{
  bool hasDirectionChanged = false;
  m_XSpeed_pps = 200;

  // Check if direction has changed
  if(m_LastDirection != JPF_JOY_RIGHT)
  {
    hasDirectionChanged = true;
    m_AnimFrameCnt = 0;
    m_LastDirection = JPF_JOY_RIGHT;

    m_HunterRightAnim.GetFirstImage();
    m_Shape.SetAnimSequence(&m_HunterRightAnim);
  }

  int dX = pps2Dist(m_XSpeed_pps, elapsed);

  if(m_Shape.XPos() + dX > m_GameView.Width() + m_Shape.Width())
  {
    m_Shape.Move(-m_Shape.Width(), m_Shape.YPos());
  }
  else
  {
    m_Shape.Move(m_Shape.XPos() + dX, m_Shape.YPos());
  }

  // Every some frames (or if the direction changed) switch the hunter
  // image
  int switchEvery = m_FrameSwitchingRateAt50Fps * 1000 / (elapsed * 50);
  m_AnimFrameCnt++;

  if (hasDirectionChanged || (m_AnimFrameCnt % switchEvery == 0))
  {
    m_AnimFrameCnt = 0;
    m_Shape.NextImage();
  }
}


void Hunter::shootArrow()
{
}


void Hunter::stopping()
{
}

