#ifndef GAME_VARS_H
#define GAME_VARS_H

#include <stddef.h>

class GameVars
{
public:
  GameVars();
  virtual ~GameVars();

  size_t MaxStrain() const;
  size_t MaxArrows() const;
  
  size_t NumArrows() const;
  void SetNumArrows(size_t numArrows);
  void DecreaseNumArrows();

private:
  const size_t m_MaxStrain;
  const size_t m_MaxArrows;
  size_t m_NumArrows;
};

#endif
