#include "Octave.h"

Octave::Octave()
  : m_pOSamps(NULL),
    m_OSizes(0),
    m_pRSamps(NULL),
    m_RSizes(0),
    m_SpCycs(0)
{

}

  Octave::Octave(BYTE* pOSamps, 
                 ULONG oSizes, 
                 BYTE* pRSamps, 
                 ULONG rSizes, 
                 ULONG spCycs)
  : m_pOSamps(pOSamps),
    m_OSizes(oSizes),
    m_pRSamps(pRSamps),
    m_RSizes(rSizes),
    m_SpCycs(spCycs)
{

}


Octave::~Octave()
{

}


BYTE* Octave::OSamps() const
{
  return m_pOSamps;
}

ULONG Octave::OSizes() const
{
  return m_OSizes;
}

BYTE* Octave::RSamps() const
{
  return m_pRSamps;
}

ULONG Octave::RSizes() const
{
  return m_RSizes;
}

ULONG Octave::SpCycs() const
{
  return m_SpCycs;
}


void Octave::SetOSamps(BYTE* pOSamps)
{
  m_pOSamps = pOSamps;
}

void Octave::SetOSizes(ULONG oSizes)
{
  m_OSizes = oSizes;
}

void Octave::SetRSamps(BYTE* pRSamps)
{
  m_pRSamps = pRSamps;
}

void Octave::SetRSizes(ULONG rSizes)
{
  m_RSizes = rSizes;
}

void Octave::SetSpCycs(ULONG spCycs)
{
  m_SpCycs = spCycs;
}
