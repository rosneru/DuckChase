#ifndef SOUND_PLAYER_H
#define SOUND_PLAYER_H

#include <stddef.h>

#include <devices/audio.h>
#include <exec/ports.h>

#include "Soundfile8SVX.h"

class SoundPlayer
{
public:
  /**
   * Opens audio device for one audio channel, 2 IO requests
   * Returns 0 for success
   *
   * Based on code by Dan Baker
   */
  SoundPlayer();
  virtual ~SoundPlayer();

  LONG PlaySample(Soundfile8SVX soundFile, 
                  LONG octave, 
                  LONG note, 
                  UWORD volume, 
                  ULONG delay);

private:
  const LONG m_NTSCClock;
  const LONG m_PALClock;
  struct MsgPort* port;
  const size_t m_NumAIOs;
  struct IOAudio** m_ppAIO;
  ULONG tclock;
};

#endif
