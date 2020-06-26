#include <clib/exec_protos.h>
#include <graphics/gfxbase.h>

#include "SoundPlayer.h"


extern struct GfxBase* GfxBase;

/* periods for scale starting at   65.40Hz (C) with 128 samples per cycle
 *                            or  130.81Hz (C) with  64 samples per cycle
 *                            or  261.63Hz (C) with  32 samples per cycle
 *                            or  523.25Hz (C) with  16 samples per cycle
 *                            or 1046.50Hz (C) with   8 samples per cycle
 *                            or 2093.00Hz (C) with   4 samples per cycle
 */

UWORD per_ntsc[12] = {428, 404, 380, 360, 340, 320,
                      302, 286, 270, 254, 240, 226};

/* periods adjusted for system clock frequency */
UWORD per[12];

SoundPlayer::SoundPlayer()
  : m_NTSCClock(3579545L),
    m_PALClock(3546895L),
    port(NULL),
    m_NumAIOs(4),
    m_ppAIO(NULL)
{
  // Ask the system if we are PAL or NTSC and set clock constant accordingly
  if(((struct GfxBase*)GfxBase)->DisplayFlags & PAL)
  {
    tclock = m_PALClock;
  }
  else
  {
    tclock = m_NTSCClock;
  }

  // Calculate period values for one octave based on system clock
  ULONG period;
  for (size_t k = 0; k < 12; k++)
  {
    period = ((per_ntsc[k] * tclock) + (m_NTSCClock >> 1)) / m_NTSCClock;
    per[k] = period;
  }

  // Create a reply port so the audio device can reply to our commands
  port = CreateMsgPort();
  if(port == NULL)
  {
    throw "SoundPlayer: Failed to create MessagePort.";
  }
  
  // Create a dynamic array for all needed IOAudio structs
  size_t aioPtrSize = sizeof(struct IOAudio**) * m_NumAIOs;
  m_ppAIO = (struct IOAudio**) AllocVec(aioPtrSize, MEMF_ANY|MEMF_CLEAR);
  if(m_ppAIO == NULL)
  {
    throw "SoundPlayer: Failed to create the array for IOAudio.";
  }

  // Create audio I/O blocks so we can send commands to the audio device
  for(size_t k = 0; k < m_NumAIOs; k++)
  {
    m_ppAIO[k] = (struct IOAudio*) CreateIORequest(port, sizeof(struct IOAudio));
    if(m_ppAIO[k] == NULL)
    {
      throw "SoundPlayer: Failed to create IORequest.";
    }
  }
}

SoundPlayer::~SoundPlayer()
{
  if(m_ppAIO != NULL)
  {
    for(size_t k = 0; k < m_NumAIOs; k++)
    {
      if(m_ppAIO[k] != NULL)
      {
        DeleteIORequest(m_ppAIO[k]);
        m_ppAIO[k] = NULL;
      }
    }

    FreeVec(m_ppAIO);
    m_ppAIO = NULL;
  }

  if(port != NULL)
  {
    DeleteMsgPort(port);
    port = NULL;
  }
}

LONG SoundPlayer::PlaySample(Soundfile8SVX soundFile, 
                             LONG octave, 
                             LONG note, 
                             UWORD volume, 
                             ULONG delay)
{
  return -1;
}