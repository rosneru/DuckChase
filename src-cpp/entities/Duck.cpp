
#include <clib/graphics_protos.h>

#include "Duck.h"

Duck::Duck(IGameView& gameView)
  : GelsBob(gameView.Depth(), 59, 21, 3), // TODO find better solution
    m_GameView(gameView),
    m_pLastError(NULL),
    m_AnimFrameCnt(1),
    m_XSpeed_pps(0),
    m_YSpeed_pps(0)
{

}

Duck::~Duck()
{

}

bool Duck::Init()
{
  //
  // Loading all the duck images
  //
  if(AddRawImage("/gfx/ente1_hires.raw") == false)
  {
    m_pLastError = "Couldn't load duck image #1 (/gfx/ente1_hires.raw).\n";
    return false;
  }

  if(AddRawImage("/gfx/ente2_hires.raw") == false)
  {
    m_pLastError = "Couldn't load duck image #2 (/gfx/ente2_hires.raw).\n";
    return false;
  }

  //
  // Initialize postion of the duck bob and add it to the scene
  //
  Move(20, 222);
  AddToRastPort(m_GameView.RastPort());

  return true;
}

void Duck::Update(unsigned long elapsed, unsigned long joyPortState)
{
  //
  // Move the duck on an easy, linear right-to-left route
  //
  m_XSpeed_pps = -4;

  if(XPos() + m_XSpeed_pps < 0)
  {
    Move(656, YPos());
  }
  else
  {
    Move(XPos() + m_XSpeed_pps, YPos());
  }

  // Change the duck image every 2 frames
  if(m_AnimFrameCnt % 4 == 0)
  {
    NextImage();
    m_AnimFrameCnt = 0;
  }

  m_AnimFrameCnt++;
}


const char* Duck::LastError() const
{
  return m_pLastError;
}


int Duck::XSpeed_pps()
{
  return m_XSpeed_pps;
}


int Duck::YSpeed_pps()
{
  return m_YSpeed_pps;
}
