#ifndef BULLET_H
#define BULLET_H

#include "HwSprite.h"
#include "Hunter.h"
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
  Bullet(IGameView& gameView, Hunter& hunter);
  ~Bullet();

  bool Init();
   void Update(unsigned long elapsed, unsigned long joyPortState);

  const char* LastError() const;

private:
  IGameView& m_GameView;
  Hunter& m_Hunter;
  struct ExtSprite* m_pSprite;
  const char* m_pLastError;
  
  int m_AnimFrameCnt;
  bool m_bInvisible;
  int m_InitialSpeed;

};

#endif
