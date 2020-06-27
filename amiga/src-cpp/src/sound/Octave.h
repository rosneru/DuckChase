#ifndef OCTAVE_H
#define OCTAVE_H

#include <exec/types.h>

class Octave
{
  Octave();
  virtual ~Octave();

	BYTE* OSamps() const;
	ULONG OSizes() const;
	BYTE* RSamps() const;
	ULONG RSizes() const;
	ULONG SpCycs() const;

	void SetOSamps(BYTE* pOSamps);
	void SetOSizes(ULONG oSizes);
	void SetRSamps(BYTE* pRSamps);
	void SetRSizes(ULONG rSizes);
	void SetSpCycs(ULONG spCycs);

private:
	BYTE* m_pOSamps;
	ULONG m_OSizes;
	BYTE* m_pRSamps;
	ULONG m_RSizes;
	ULONG m_SpCycs;
};

#endif
