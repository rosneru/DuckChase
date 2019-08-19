#ifndef HUNTER_H
#define HUNTER_H

#include "GelsBob.h"
#include "IGameView.h"

/**
 * The hunter.
 *
 *
 * @author Uwe Rosner
 * @date 11/08/2019
 */
class Hunter : public GelsBob
{
public:
  Hunter(IGameView& gameView);
  ~Hunter();

  bool Init();
   void Update(unsigned long elapsed, unsigned long joyPortState);

  const char* LastError() const;

  /**
   * Returns the hunter's speed in xdirection in pixel per second.
   * Negative for left movement, zero if no movement.
   */
  int XSpeed_pps();

  int XPos();
  int YPos();

private:
  IGameView& m_GameView;
  struct Bob* m_pBob;
  const char* m_pLastError;
  int m_AnimFrameCnt;

  int m_XSpeed_pps;

};

#endif
