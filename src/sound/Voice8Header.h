#ifndef VOICE_8_HEADER_H
#define VOICE_8_HEADER_H

#include <exec/types.h>

#define sCmpNone       0  // not compressed
#define sCmpFibDelta   1  // Fibonacci-delta encoding; see RKRM Devices, Appendix C

/**
 * Description of the VHDR iff chunk content as defined in Amiga Rom
 * Kernel Reference Manual 'Devices'.
 */
struct Voice8Header
{
  /**
   * # samples in the high octave 1-shot part
   */
  ULONG oneShotHiSamples;

  /**
   * # samples in the high octave repeat part
   */
  ULONG repeatHiSamples;

  /**
   * # samples/cycle in high octave, else 0
   */
  ULONG samplesPerHiCycle;

  /**
   * data sampling rate
   */
  UWORD samplesPerSec;

  /**
   * # of octaves of waveforms
   */
  UBYTE ctOctave;

  /**
   * data compression technique used
   */
  UBYTE sCompression;

  /**
   * playback nominal volume from 0 to Unity (full volume). Map this
   * value into the output hardware's dynamic range.
   */
  LONG volume;
        
};

#endif
