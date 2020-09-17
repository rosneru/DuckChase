#include <clib/exec_protos.h>
#include <exec/memory.h>
#include <graphics/view.h>

#include "IffParse.h"
#include "SaveBitMapPictureIlbm.h"

// Flags that should be masked out of old 16-bit CAMG before save or
// use. Note that 32-bit mode id (non-zero high word) bits should not be
// twiddled
#define BADFLAGS  (SPRITES|VP_HIDE|GENLOCK_AUDIO|GENLOCK_VIDEO)
#define OLDCAMGMASK  (~BADFLAGS)


SaveBitMapPictureIlbm::SaveBitMapPictureIlbm(const BitMapPictureBase& picture, 
                                             const char* pFileName)
  : m_BODY_BUF_SIZE(5004),
    m_ModeId(picture.GetModeId()),
    bodybuf(NULL)
{
  if (!(m_ModeId & 0xFFFF0000))
  {
    m_ModeId &= OLDCAMGMASK;
  }

  if (!(bodybuf = (UBYTE*) AllocMem(m_BODY_BUF_SIZE, MEMF_PUBLIC)))
  {
    cleanup();
    throw "SaveBitMapPictureIlbm: Failed to allocate buffer.";
  }

  IffParse iffParse(pFileName, true);
}

SaveBitMapPictureIlbm::~SaveBitMapPictureIlbm()
{
  cleanup();
}

void SaveBitMapPictureIlbm::cleanup()
{
  if(bodybuf != NULL)
  {
    FreeVec(bodybuf);
    bodybuf = NULL;
  }
}
