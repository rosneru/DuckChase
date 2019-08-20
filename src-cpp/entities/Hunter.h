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

  int XSpeed_pps();
  int YSpeed_pps();

private:
  IGameView& m_GameView;
  const char* m_pLastError;
  int m_AnimFrameCnt;

  int m_XSpeed_pps;
  int m_YSpeed_pps;

};

#endif
