#ifndef BULLET_H
#define BULLET_H

#include "HwSprite.h"
#include "IGameView.h"

/**
 * The bullet.
 *
 *
 * @author Uwe Rosner
 * @date 11/08/2019
 */
class Bullet : public HwSprite
{
public:
  Bullet(IGameView& gameView);
  ~Bullet();

  bool Init();
   void Update(unsigned long elapsed, unsigned long joyPortState);

  const char* LastError() const;

private:
  IGameView& m_GameView;
  struct ExtSprite* m_pSprite;
  const char* m_pLastError;
  int m_AnimFrameCnt;

};

#endif
