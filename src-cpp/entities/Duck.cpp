
#include <clib/graphics_protos.h>

#include "Duck.h"

Duck::Duck(IGameView& gameView)
  : GelsBob(gameView.Depth(), 59, 21, 3), // TODO find better solution
    m_GameView(gameView),
    m_pBob(NULL),
    m_pLastError(NULL),
    m_AnimFrameCnt(1) // TODO CHECK Why not 0?
{

}

Duck::~Duck()
{
  if(m_pBob != NULL)
  {
    RemBob(m_pBob);
    m_pBob = NULL;
  }
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
  // Trying to ackquire the bob
  //
  m_pBob = Get();

  if(m_pBob == NULL)
  {
    m_pLastError = "Couldn't acquire the duck bob.\n";
    return false;
  }

  //
  // Initialize postion of the duck bob and add it to the scene
  //
  m_pBob->BobVSprite->X = 200;
  m_pBob->BobVSprite->Y = 40;

  AddBob(m_pBob, m_GameView.RastPort());

  return true;
}

void Duck::Update(unsigned long elapsed, unsigned long joyPortState)
{
  //
  // Move the duck on an easy, linear right-to-left route
  //
  m_pBob->BobVSprite->X -= 4;
  if(m_pBob->BobVSprite->X < -40)
  {
    m_pBob->BobVSprite->X = 650;
  }

  // Change the duck image every 2 frames
  if(m_AnimFrameCnt % 4 == 0)
  {
    NextImage();
    InitMasks(m_pBob->BobVSprite);

    m_AnimFrameCnt = 0;
  }

  m_AnimFrameCnt++;
}

const char* Duck::LastError() const
{
  return m_pLastError;
}
