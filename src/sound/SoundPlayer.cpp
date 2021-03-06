#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <graphics/gfxbase.h>

#include "SoundPlayer.h"


extern struct GfxBase* GfxBase;

// Max playable sample in one IO request is 128K
#define MAXSAMPLE 131072

UBYTE whichannel[] = {1, 2, 4, 8};

// periods for scale starting at   65.40Hz (C) with 128 samples per cycle
//                            or  130.81Hz (C) with  64 samples per cycle
//                            or  261.63Hz (C) with  32 samples per cycle
//                            or  523.25Hz (C) with  16 samples per cycle
//                            or 1046.50Hz (C) with   8 samples per cycle
//                            or 2093.00Hz (C) with   4 samples per cycle
// 

UWORD per_ntsc[12] = {428, 404, 380, 360, 340, 320,
                      302, 286, 270, 254, 240, 226};


SoundPlayer::SoundPlayer()
  : m_NTSCClock(3579545L),
    m_PALClock(3546895L),
    m_Clock(m_NTSCClock),
    m_ReplyPort(NULL),
    m_NumAIOs(4),
    m_ppAIO(NULL),
    m_IsAudioDeviceOpened(false)
{
  // If we are on a PAL Amiga set clock constant accordingly
  if(((struct GfxBase*)GfxBase)->DisplayFlags & PAL)
  {
    m_Clock = m_PALClock;
  }

  // Calculate period values for one octave based on system clock
  ULONG period;
  for (size_t k = 0; k < 12; k++)
  {
    period = ((per_ntsc[k] * m_Clock) + (m_NTSCClock >> 1)) / m_NTSCClock;
    m_Periods[k] = period;
  }

  // Create a reply port so the audio device can reply to our commands
  m_ReplyPort = CreateMsgPort();
  if(m_ReplyPort == NULL)
  {
    throw "SoundPlayer: Failed to create MessagePort.";
  }
  
  // Create a dynamic array for all needed IOAudio structs
  size_t aioPtrSize = sizeof(struct IOAudio*) * m_NumAIOs;
  m_ppAIO = (struct IOAudio**) AllocVec(aioPtrSize, MEMF_ANY|MEMF_CLEAR);
  if(m_ppAIO == NULL)
  {
    throw "SoundPlayer: Failed to create the array for IOAudio.";
  }

  // Create audio I/O blocks so we can send commands to the audio device
  for(size_t k = 0; k < m_NumAIOs; k++)
  {
    m_ppAIO[k] = (struct IOAudio*) CreateIORequest(m_ReplyPort, 
                                                   sizeof(struct IOAudio));
    if(m_ppAIO[k] == NULL)
    {
      throw "SoundPlayer: Failed to create IORequest.";
    }
  }

  //
  // Set up the audio I/O block for channel allocation
  //

  m_ppAIO[0]->ioa_Request.io_Command = ADCMD_ALLOCATE;
  m_ppAIO[0]->ioa_Request.io_Flags = ADIOF_NOWAIT;
  m_ppAIO[0]->ioa_AllocKey = 0; // will be filled in by the audio device
                                // if the allocation succeeds. We must 
                                // use the key it gives for all other 
                                // commands sent.
  m_ppAIO[0]->ioa_Data = whichannel;  // a pointer to the array listing 
                                      // the channels we want
  m_ppAIO[0]->ioa_Length = sizeof(whichannel);  // Lenght of array

  if(OpenDevice(AUDIONAME, 0L, (struct IORequest*)m_ppAIO[0], 0L) != 0)
  {
    throw "SoundPlayer: Failed to open audio.device.";
  }

  m_IsAudioDeviceOpened = true;

  // Clone the flags, channel allocation, etc. into other IOAudio requests
  for (size_t k = 1; k < m_NumAIOs; k++)
  {
    *m_ppAIO[k] = *m_ppAIO[0];
  }
}

SoundPlayer::~SoundPlayer()
{
  if(m_IsAudioDeviceOpened)
  {
    CloseDevice((struct IORequest*)m_ppAIO[0]);
    m_IsAudioDeviceOpened = false;
  }

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

  if(m_ReplyPort != NULL)
  {
    DeleteMsgPort(m_ReplyPort);
    m_ReplyPort = NULL;
  }
}

bool SoundPlayer::PlaySample(const Soundfile8SVX& soundFile, 
                             LONG octave, 
                             LONG note, 
                             UWORD volume, 
                             ULONG delay)
{
  if(!m_IsAudioDeviceOpened)
  {
    return -1;
  }

  if(note > 11)
  {
    note = 0;
  }

  ULONG period;
  if(note == -1)
  {
    period = m_Clock / soundFile.SamplesPerSec();
  }
  else
  {
    // table 'm_Periods' set up in constructor to system clock frequency
    period = m_Periods[note];
  }

  if(octave > soundFile.NumOctaves())
  {
    octave = 0;
  }

  if(volume > 64)
  {
    volume = 64;
  }

  Octave* pOctave = soundFile.GetOctave(octave);
  
  UBYTE* pOneShot = (UBYTE*) pOctave->OSamps();
  LONG oSize = pOctave->OSizes();
  UBYTE* pRepeat = (UBYTE*) pOctave->RSamps();
  LONG rSize = pOctave->RSizes();

  // Set up audio I/O blocks to play a sample using CMD_WRITE. Set up
  // one request for the oneshot and one for repeat (all ready for
  // simple case, but we may not need both) The io_Flags are set to
  // ADIOF_PERVOL so we can set the period (speed) and volume with the
  // our sample; ioa_Data points to the sample; ioa_Length gives the
  // length ioa_Cycles tells how many times to repeat the sample If you
  // want to play the sample at a given sampling rate, set ioa_Period =
  // clock/(given sampling rate)

  m_ppAIO[0]->ioa_Request.io_Command = CMD_WRITE;
  m_ppAIO[0]->ioa_Request.io_Flags = ADIOF_PERVOL;
  m_ppAIO[0]->ioa_Data = pOneShot;
  m_ppAIO[0]->ioa_Length = oSize;
  m_ppAIO[0]->ioa_Period = period;
  m_ppAIO[0]->ioa_Volume = volume;
  m_ppAIO[0]->ioa_Cycles = 1;

  m_ppAIO[2]->ioa_Request.io_Command = CMD_WRITE;
  m_ppAIO[2]->ioa_Request.io_Flags = ADIOF_PERVOL;
  m_ppAIO[2]->ioa_Data = pRepeat;
  m_ppAIO[2]->ioa_Length = rSize;
  m_ppAIO[2]->ioa_Period = period;
  m_ppAIO[2]->ioa_Volume = volume;
  m_ppAIO[2]->ioa_Cycles = 0; // repeat until stopped

  // Send the command to start a sound using BeginIO() Go to sleep and
  // wait for the sound to finish with WaitIO() to wait and get the get
  // the ReplyMsg

  struct IOAudio* aout0 = NULL;
  struct IOAudio* aout1 = NULL;

  if(oSize != 0)
  {
    if(oSize < MAXSAMPLE)
    {
      // Simple case for oneshot sample <= 128K (ie. most samples)
      aout0 = m_ppAIO[0];
      BeginIO((struct IORequest*) aout0);
    }
    else
    {
      // for samples >128K some advanced technique is used
      *m_ppAIO[1] = *m_ppAIO[1];
      // aout0 = playbigsample(m_ppAIO[0], m_ppAIO[1], pOneShot, oSize, period, volume);
    }
  }

  if(rSize != 0)
  {
    if(rSize < MAXSAMPLE)
    {
      // Simple case for oneshot sample <= 128K (ie. most samples)
      aout1 = m_ppAIO[2];
      BeginIO((struct IORequest*) aout1);
    }
    else
    {
      // for samples >128K some advanced technique is used
      *m_ppAIO[3] = *m_ppAIO[2];
      // aout0 = playbigsample(m_ppAIO[2], m_ppAIO[3], pOneShot, oSize, period, volume);
    }
  }

  if(delay != 0)
  {
    // Crude timing for notes
    Delay(delay);
  }

  // Wait for any requests we still have out
  if(aout0 != NULL)
  {
    WaitIO((struct IORequest*) aout0);
  }

  if(aout1 != NULL)
  {
    if(note >= 0)
    {
      // Stop current note now
      AbortIO((struct IORequest*) aout1);
    }

    WaitIO((struct IORequest*) aout1);
  }

  return 0;
}
