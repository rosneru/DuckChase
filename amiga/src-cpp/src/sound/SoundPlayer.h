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

  /**
   * Play a note in octave for delay/50ths of a second OR Play a sound
   * effect (set octave and note to 0, -1)
   * 
   * When playing notes:
   * - Expects note values between 0 (C) and 11 (B#)
   * - Uses largest octave sample in 8SVX as octave 0, next smallest
   *   as octave 1, etc.
   * 
   * NOTE:
   * - this simple example routine does not do ATAK and RLSE)
   * - use of Delay for timing is simplistic, synchronous, and does not 
   *   take into account that the oneshot itself may be longer than the 
   *   delay.
   * - Use timer.device for more accurate asynchronous delays
   */
  bool PlaySample(const Soundfile8SVX& soundFile, 
                  LONG octave, 
                  LONG note, 
                  UWORD volume, 
                  ULONG delay);

private:
  const LONG m_NTSCClock;
  const LONG m_PALClock;
  ULONG m_Clock;
  UWORD m_Periods[12];
  struct MsgPort* m_ReplyPort;
  const size_t m_NumAIOs;
  struct IOAudio** m_ppAIO;
  bool m_IsAudioDeviceOpened;
};

#endif
