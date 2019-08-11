#ifndef DUCK_H
#define DUCK_H

#include "GelsBob.h"
#include "IGameView.h"

/**
 * The duck.
 *
 *
 * @author Uwe Rosner
 * @date 11/08/2019
 */
class Duck : public GelsBob
{
public:
  Duck(IGameView& gameView);
  ~Duck();

  bool Init();
   void Update(unsigned long elapsed, unsigned long joyPortState);

  const char* LastError() const;

private:
  IGameView& m_GameView;
  struct Bob* m_pBob;
  const char* m_pLastError;
  int m_AnimFrameCnt;

};

#endif
