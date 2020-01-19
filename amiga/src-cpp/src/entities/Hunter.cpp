
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "Hunter.h"

Hunter::Hunter(IGameView& gameView)
  : EntityBase(&m_Shape),
    m_GameView(gameView),
    m_Shape(3, 16, 22, 3), // TODO find better solution
    m_pLastError(NULL),
    m_AnimFrameCnt(1),  // TODO CHECK Why not 0?
    m_XSpeed_pps(0),
    m_YSpeed_pps(0)
{

}

Hunter::~Hunter()
{

}

bool Hunter::Init()
{
  //
  // Loading all the hunter images
  //
  if(m_Shape.AddRawImage("gfx/hunter1_hires.raw") == false)
  {
    m_pLastError = "Couldn't load duck image #1 (gfx/hunter1_hires.raw).\n";
    return false;
  }

  if(m_Shape.AddRawImage("gfx/hunter2_hires.raw") == false)
  {
    m_pLastError = "Couldn't load duck image #2 (gfx/hunter2_hires.raw).\n";
    return false;
  }

  //
  // Initialize postion of the duck bob and add it to the scene
  //
  m_Shape.AddToRastPort(m_GameView.RastPort());
  m_Shape.Move(20, 222);

  return true;
}

void Hunter::Update(unsigned long elapsed, unsigned long joyPortState)
{
  if((joyPortState & JPF_JOY_RIGHT) != 0)
  {
    m_XSpeed_pps = 500;
    int dX = pps2Dist(m_XSpeed_pps, elapsed);

    if(m_Shape.XPos() + dX > 640 + m_Shape.Width())
    {
      m_Shape.Move(-m_Shape.Width(), m_Shape.YPos());
    }
    else
    {
      m_Shape.Move(m_Shape.XPos() + dX, m_Shape.YPos());
    }

  }
  else if((joyPortState & JPF_JOY_LEFT) != 0)
  {
    m_XSpeed_pps = -500;
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