#include <clib/graphics_protos.h>

#include "Duck.h"

Duck::Duck(IGameView& gameView)
  : EntityBase(&m_Shape),
    m_GameView(gameView),
    m_Shape(3, 59, 21, 3), // TODO find better solution
    m_DuckAnim(3, 59, 21),
    m_AnimFrameCnt(1),
    m_pLastError(NULL),
    m_XSpeed_pps(0),
    m_YSpeed_pps(0)
{

}

Duck::~Duck()
{

}

bool Duck::Init()
{
  const char* ppFiles[]  = {"gfx/duck1_hires.raw", 
                            "gfx/duck2_hires.raw",
                            NULL};

  if(m_DuckAnim.Load(ppFiles) == false)
  {
    m_pLastError = m_DuckAnim.ErrorMsg();
    return false;
  }

  //
  // Loading all the duck images
  //
  if(m_Shape.AddRawImage("gfx/duck1_hires.raw") == false)
  {
    m_pLastError = "Couldn't load duck image #1 (gfx/duck1_hires.raw).\n";
    return false;
  }

  if(m_Shape.AddRawImage("gfx/duck2_hires.raw") == false)
  {
    m_pLastError = "Couldn't load duck image #2 (gfx/duck2_hires.raw).\n";
    return false;
  }

  //
  // Initialize postion of the duck bob and add it to the scene
  //
  m_Shape.AddToRastPort(m_GameView.RastPort());
  m_Shape.Move(200, 40);

  return true;
}

void Duck::Update(unsigned long elapsed, unsigned long joyPortState)
{
  //
  // Move the duck on an easy, linear right-to-left route
  //
  m_XSpeed_pps = -240;
  int dX = pps2Dist(m_XSpeed_pps, elapsed);

  if(XPos() + dX < -m_Shape.Width())
  {
    m_Shape.Move(656, m_Shape.YPos());
  }
  else
  {
    m_Shape.Move(m_Shape.XPos() + dX, m_Shape.YPos());
  }

  // Change the duck image every 2 frames
  if(m_AnimFrameCnt % 4 == 0)
  {
    m_Shape.NextImage();
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
