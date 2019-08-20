
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "Hunter.h"

Hunter::Hunter(IGameView& gameView)
  : GelsBob(gameView.Depth(), 16, 22, 3), // TODO find better solution
    m_GameView(gameView),
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
  if(AddRawImage("/gfx/jaeger1_hires.raw") == false)
  {
    m_pLastError = "Couldn't load duck image #1 (/gfx/jaeger1_hires.raw).\n";
    return false;
  }

  if(AddRawImage("/gfx/jaeger2_hires.raw") == false)
  {
    m_pLastError = "Couldn't load duck image #2 (/gfx/jaeger2_hires.raw).\n";
    return false;
  }

  //
  // Initialize postion of the duck bob and add it to the scene
  //
  Move(20, 222);
  AddToRastPort(m_GameView.RastPort());

  return true;
}

void Hunter::Update(unsigned long elapsed, unsigned long joyPortState)
{
  if((joyPortState & JP_TYPE_MASK) == JP_TYPE_JOYSTK)
  {
    if((joyPortState & JPF_JOY_RIGHT) != 0)
    {
      m_XSpeed_pps = 8;

      if(XPos() + m_XSpeed_pps > 640)
      {
        Move(-16, YPos());
      }
      else
      {
        Move(XPos() + m_XSpeed_pps, YPos());
      }
      
    }
    else if((joyPortState & JPF_JOY_LEFT) != 0)
    {
      m_XSpeed_pps = -8;

      if(XPos() + m_XSpeed_pps < 0)
      {
        Move(656, YPos());
      }
      else
      {
        Move(XPos() + m_XSpeed_pps, YPos());
      }
    }
    else
    {
      m_XSpeed_pps = 0;
    }
    
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