
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "Hunter.h"

Hunter::Hunter(IGameView& gameView)
  : GelsBob(gameView.Depth(), 16, 22, 3), // TODO find better solution
    m_GameView(gameView),
    m_pBob(NULL),
    m_pLastError(NULL),
    m_AnimFrameCnt(1) // TODO CHECK Why not 0?
{

}

Hunter::~Hunter()
{
  if(m_pBob != NULL)
  {
    RemBob(m_pBob);
    m_pBob = NULL;
  }
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
  // Trying to ackquire the bob
  //
  m_pBob = Get();

  if(m_pBob == NULL)
  {
    m_pLastError = "Couldn't acquire the hunter bob.\n";
    return false;
  }

  //
  // Initialize postion of the duck bob and add it to the scene
  //
  m_pBob->BobVSprite->X = 20;
  m_pBob->BobVSprite->Y = 222;

  AddBob(m_pBob, m_GameView.RastPort());

  return true;
}

void Hunter::Update(unsigned long elapsed, unsigned long joyPortState)
{
  if((joyPortState & JP_TYPE_MASK) == JP_TYPE_JOYSTK)
  {
    if((joyPortState & JPF_JOY_RIGHT) != 0)
    {
      m_pBob->BobVSprite->X += 8;
      if(m_pBob->BobVSprite->X > 640)
      {
        m_pBob->BobVSprite->X = -16;
      }
    }
    else if((joyPortState & JPF_JOY_LEFT) != 0)
    {
      m_pBob->BobVSprite->X -= 8;
      if(m_pBob->BobVSprite->X < 0)
      {
        m_pBob->BobVSprite->X = 656;
      }
    }
  }
}
