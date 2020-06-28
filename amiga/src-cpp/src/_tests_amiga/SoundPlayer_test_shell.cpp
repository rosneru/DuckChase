/**
 * SoundPlayer test
 * Uses some (currently under construction) cpp classes to load an 8svx
 * iff file and play it with the audio.device.
 *
 * 24.06.2020
 * Author: Uwe Rosner https://github.com/rosneru
 */
#include <stdlib.h>
#include <stdio.h>

#include <dos/dos.h>

#include "Soundfile8SVX.h"
#include "SoundPlayer.h"

int main(int argc, char** argv)
{
  try
  {
    Soundfile8SVX soundfile("AADevDuck:assets/snd/finger_snap.8svx");
    SoundPlayer player;

    player.PlaySample(soundfile, 0, -1, 64, 0);

  }
  catch(const char* pMsg)
  {
    printf("%s\n<Return>", pMsg);
    getc(stdin);
    return RETURN_OK;
  }

  printf("Finished sound player test.\n<Return>");
  getc(stdin);
  exit(RETURN_OK);
}
