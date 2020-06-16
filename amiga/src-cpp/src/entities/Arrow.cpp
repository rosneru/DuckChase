
#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>

#include "Arrow.h"

Arrow::Arrow(GameViewBase& gameView,
             const GameWorld& gameWorld,
             const ArrowResources& arrowResources,
             Duck& duck,
             size_t& isStrain,
             bool& isArrowFlightFinished,
             bool& strike)
  : EntityBase(gameWorld, m_ArrowShape, m_Animator.CurrentShadowMask()),
    m_GameView(gameView),
    m_Resources(arrowResources),
    m_Duck(duck),
    m_Strain(isStrain),
    m_IsArrowFlightFinished(isArrowFlightFinished),
    m_IsStrike(strike),
    m_ArrowShape(m_GameView.ViewPort(), 
            arrowResources),
    m_Animator(m_ArrowShape, arrowResources.AnimRightUpward()),
    m_X0(0),
    m_Y0(0)
{
  m_ArrowShape.SetVPortColorsForSprite(gameView.ViewPort(), 
                                       m_Resources.AnimRightUpward()->GetColors32());
  m_Shape.SetInvisible();
}


Arrow::~Arrow()
{

}


void Arrow::Activate(int x, int y, long xSpeed, long ySpeed)
{
  m_X0 = x;
  m_Y0 = y;

  // Here xSpeed is only a hint: When > 0, arrow is armed in right
  // direction, else in left direction
  if(xSpeed > 0)
  {
    m_Animator.SetAnimSeq(m_Resources.AnimRightUpward());
    m_XSpeed = 3 * m_Strain;
  }
  else
  {
    m_Animator.SetAnimSeq(m_Resources.AnimLeftUpward());
    m_XSpeed = -3 * m_Strain;
  } 

  m_YSpeed = m_Strain / 3 + 40;

  // Select frame of upward arrow which currently is at index 1
  m_Animator.IndexedFrame(1);
  m_Shape.Move(x, y);
  m_IsAlive = true;
  m_Action = MoveUp;

  // Calculate constants for arrow parabel
  // Formula for y-position of arrow is
  //   y = x * tan(beta) - x ^ 2 * g / (2 * v0^2 * cos^2(beta))
  // 
  // with:
  //  beta is defined to 60°, so
  //    tan(beta) is approximated to 2. (tan(60°) = 1.73)
  //    cos^2(beta) is 0.25 or 1 / 4
  //
  // So, by pre-calculating the constants a and b it can be written as:
  //   y = a * x + b * x^2
  //
  // These constants a and b are now calculated
  m_A = 2; // tan(60°)
  m_B = (20 << 16) / (m_YSpeed * m_YSpeed); // 20 = g / (2 * cos^2(60°)) 
                                            //    = 10 / (2 * 0.25) 
                                            //    = 10 / (2 * 1 / 4)

  // NOTE: (20 << 16) is done to multiply 20 by 65536 before it is
  // divided by the big square result. So its avoided that the integer
  // result is cut to zero when it should't. This is undone in Update()
  // when m_B is actually used.
}

void Arrow::Deactivate()
{
  m_IsAlive = false;
  m_Shape.SetInvisible();
}


void Arrow::Update(unsigned long elapsed, unsigned long joyPortState)
{
  if(!m_IsAlive)
  {
    return;
  }

  if(m_Shape.IsGone())
  {
    m_Shape.SetInvisible();
    m_IsAlive = false;  
    m_IsArrowFlightFinished = true;
    return;
  }

  int xAbs; // x-value already traveled by the arrow. 
            // Here always positive when moving to left.

  int dX = pps2Dist(m_XSpeed, elapsed);
  if(dX >= 0)
  {
    xAbs = m_Shape.Left() - m_X0 + dX;
  }
  else
  {
    xAbs = m_X0 - m_Shape.Left() - dX; 
  }

  //
  // Formula for y-position of arrow is
  //   y = x * tan(beta) - x ^ 2 * g / (2 * v0^2 * cos^2(beta))
  // 
  // With pre-calculating the constants (see in Activate()) it is 
  // simplified to:
  //   y = a * x + b * x^2
  //
  int y = m_A * xAbs - ((m_B * xAbs * xAbs) >> 16); // >> 16: undo the correction from Activate()
  int newY = m_Y0 - y;

  if((m_Action == MoveUp) && (newY > m_Shape.Top()))
  {
    m_Action = MoveDown;

    // Select frame of sinking arrow which currently is at index 3
    m_Animator.IndexedFrame(3);
  }

  m_Shape.Move(m_Shape.Left() + dX, newY);

  if(isCollision(m_Duck))
  {
    m_IsStrike = true;
  }
}
