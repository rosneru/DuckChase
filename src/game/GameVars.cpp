#include "GameVars.h"

GameVars::GameVars()
  : m_MaxStrain(94),
    m_MaxArrows(3),
    m_NumArrows(m_MaxArrows)
{

}

GameVars::~GameVars()
{
}


size_t GameVars::MaxStrain() const
{
  return m_MaxStrain;
}

size_t GameVars::MaxArrows() const
{
  return m_MaxArrows;
}

size_t GameVars::NumArrows() const
{
  return m_NumArrows;
}

void GameVars::SetNumArrows(size_t numArrows)
{
  m_NumArrows = numArrows;
}

void GameVars::DecreaseNumArrows()
{
  if(m_NumArrows > 0)
  {
    m_NumArrows--;
  }
}
