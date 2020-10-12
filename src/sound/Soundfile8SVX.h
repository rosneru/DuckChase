#ifndef SOUNDFILE_8SVX_H
#define SOUNDFILE_8SVX_H

#include "IffParse.h"
#include "Octave.h"
#include "Voice8Header.h"


/**
 * Class for loading an IFF 8SVX sound file using the iffparse.library.
 *  
 * Code of newiff from Commodore is heavily used.
 *
 * @author Uwe Rosner
 * @date 24/06/2020
 */
class Soundfile8SVX
{
public:
  /**
   * Opens and reads an iff 8svx sound file with given name.
   */
  Soundfile8SVX(const char* pFileName);

  virtual ~Soundfile8SVX();

  /**
   *  Data sampling rate
   * 
   *  IMPORTANT: Move into a base class if more audio formats will be 
   *             implemented.
   */
  UWORD SamplesPerSec() const;

  /**
   *  Number of octaves of waveforms
   * 
   *  IMPORTANT: Move into a base class if more audio formats will be 
   *             implemented.
   */
  UBYTE NumOctaves() const;

  Octave* GetOctave(size_t i) const;

private:
  BYTE* m_pSampleData;
  ULONG m_NumSampleBytes;
  Octave** m_ppOctaves;

  UWORD m_SamplesPerSec;
  UBYTE m_NumOctaves;

  bool decode8SVXBody(IffParse& iffParse, struct Voice8Header* pHdr);

  /** 
   * Unpack Fibonacci-delta encoded data from n byte source buffer into
   * 2*(n-2) byte dest buffer. Source buffer has a pad byte, an 8-bit
   * initial value, followed by n-2 bytes comprising 2*(n-2) 4-bit
   * encoded samples.
   */
  void DUnpack(BYTE source[], LONG n, BYTE dest[]);

  /**
   * Unpack Fibonacci-delta encoded data from n byte source buffer into
   * 2*n byte dest buffer, given initial data value x.  It returns the
   * lats data value x so you can call it several times to incrementally
   * decompress the data.
   */
  BYTE D1Unpack(BYTE source[], LONG n, BYTE dest[], BYTE x);
};

#endif
