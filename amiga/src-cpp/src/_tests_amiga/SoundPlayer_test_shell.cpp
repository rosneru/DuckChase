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

int main(int argc, char** argv)
{
  printf("Finished sound player test.\n<Return>");
  getc(stdin);
  exit(RETURN_OK);
}
