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
   * Creates a SoundPlayer by opening the audio device for one audio 
   * channel, 2 IO requests.
   *
   * Based on code by Dan Baker; Code of newiff from Commodore is used.
   * 
   * @author Uwe Rosner
   * @date 26/06/2020
   */
  SoundPlayer();
  virtual ~SoundPlayer();

  LONG PlaySample(const Soundfile8SVX& soundFile, 
                  LONG octave, 
                  LONG note, 
                  UWORD volume, 
                  ULONG delay);

private:
  const LONG m_NTSCClock;
  const LONG m_PALClock;
  ULONG m_Clock;
  struct MsgPort* m_ReplyPort;
  const size_t m_NumAIOs;
  struct IOAudio** m_ppAIO;
  bool m_IsAudioDeviceOpened;
};

#endif
