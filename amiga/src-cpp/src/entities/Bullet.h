#ifndef BULLET_H
#define BULLET_H

#include "ShapeSprite.h"
#include "Hunter.h"
#include "IGameView.h"

/**
 * The bullet.
 *
 *
 * @author Uwe Rosner
 * @date 11/08/2019
 */
class Bullet
{
public:
  Bullet(IGameView& gameView, Hunter& hunter);
  ~Bullet();

  bool Init();

  void Update(unsigned long elapsed, unsigned long joyPortState);

  const char* LastError() const;

  int XSpeed_pps();
  int YSpeed_pps();

private:
  IGameView& m_GameView;
  Hunter& m_Hunter;
  ShapeSprite m_Sprite;

  const char* m_pLastError;

  int m_AnimFrameCnt;

  int m_XSpeed_pps;
  int m_YSpeed_pps;

  /**
   * Returns the distance in pixels which is calculated from the pixel-
   * per-second value und the elapsed time.
   */
  int pps2Dist(int pps, long elapsed_ms);
};

#endif
