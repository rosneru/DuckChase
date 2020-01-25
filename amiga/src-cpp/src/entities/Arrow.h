#ifndef BULLET_H
#define BULLET_H

#include "AnimSeqSprite.h"
#include "EntityBase.h"
#include "Hunter.h"
#include "GameColors.h"
#include "IGameView.h"
#include "ShapeSprite.h"

/**
 * The bullet.
 *
 *
 * @author Uwe Rosner
 * @date 11/08/2019
 */
class Arrow : public EntityBase
{
public:
  Arrow(IGameView& gameView, GameColors& colors, Hunter& hunter);
  ~Arrow();

  bool Init();

  void Update(unsigned long elapsed, unsigned long joyPortState);

  const char* LastError() const;

  int XSpeed_pps();
  int YSpeed_pps();

private:
  IGameView& m_GameView;
  GameColors& m_GameColors;
  Hunter& m_Hunter;

  ShapeSprite m_Shape;
  AnimSeqSprite m_ArrowAnimSeq;

  const char* m_pLastError;

  int m_AnimFrameCnt;

  int m_XSpeed_pps;
  int m_YSpeed_pps;
};

#endif
