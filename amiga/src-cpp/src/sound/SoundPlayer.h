#ifndef SOUND_PLAYER_H
#define SOUND_PLAYER_H

#include "Soundfile8SVX.h"

class SoundPlayer
{
public:
  SoundPlayer();
  virtual ~SoundPlayer();

  LONG PlaySample(Soundfile8SVX soundFile, 
                  LONG octave, 
                  LONG note, 
                  UWORD volume, 
                  ULONG delay);
};

#endif
