#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/iffparse_protos.h>

#include <exec/memory.h>
#include <graphics/view.h>
#include <libraries/iffparse.h>

#include "IffParse.h"
#include "SaveBitMapPictureIlbm.h"

// Flags that should be masked out of old 16-bit CAMG before save or
// use. Note that 32-bit mode id (non-zero high word) bits should not be
// twiddled
#define BADFLAGS  (SPRITES|VP_HIDE|GENLOCK_AUDIO|GENLOCK_VIDEO)
#define OLDCAMGMASK  (~BADFLAGS)


SaveBitMapPictureIlbm::SaveBitMapPictureIlbm(const BitMapPictureBase& picture, 
                                             const char* pFileName)
  : m_Picture(picture),
    m_BODY_BUF_SIZE(5004),
    m_ModeId(picture.GetModeId()),
    m_Bmhd(),
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

  IffParse iff(pFileName, true);
  
  LONG err = PushChunk(iff.Handle(), ID_ILBM, ID_FORM, IFFSIZE_UNKNOWN);
  if(err != 0)
  {
    throw "SaveBitMapPictureIlbm: Failed to write FORM chunk.";
  }

  initBitMapHeader();
  err = iff.PutCk(ID_BMHD, sizeof(struct BitMapHeader), (BYTE*) &m_Bmhd);
  if(err != 0)
  {
    throw "SaveBitMapPictureIlbm: Failed to write BitMapHeader.";
  }

  if(m_Picture.GetColors32() != NULL)
  {
    err = PutCmap(iff.Handle());
    if(err != 0)
    {
      throw "SaveBitMapPictureIlbm: Failed to write ColorMap.";
    }
  }
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


void SaveBitMapPictureIlbm::initBitMapHeader()
{
  struct DisplayInfo DI;

  m_Bmhd.bmh_Width = m_Picture.Width();
  m_Bmhd.bmh_Height = m_Picture.Height();
  m_Bmhd.bmh_Left = m_Bmhd.bmh_Top = 0;   // Default position is (0,0)
  m_Bmhd.bmh_Depth = m_Picture.Depth();
  m_Bmhd.bmh_Masking = mskHasMask;
  m_Bmhd.bmh_Compression = cmpByteRun1;
  m_Bmhd.bmh_Transparent = 0;
  m_Bmhd.bmh_PageWidth = m_Picture.Width() < 320 ? 320 : m_Picture.Width();
  m_Bmhd.bmh_PageHeight = m_Picture.Height() < 200 ? 200 :  m_Picture.Height();
  m_Bmhd.bmh_XAspect = 0;
  m_Bmhd.bmh_YAspect = 0;

  if (GetDisplayInfoData(NULL, (UBYTE*)&DI, sizeof(struct DisplayInfo),
                          DTAG_DISP, m_Picture.GetModeId()))
  {
    m_Bmhd.bmh_XAspect = DI.Resolution.x;
    m_Bmhd.bmh_YAspect = DI.Resolution.y;
  }
}


long SaveBitMapPictureIlbm::PutCmap(struct IFFHandle* iff)
{
  long error;
  ULONG* pTableEntry;
  ColorRegister cmapReg;
  ULONG ncolors = 1L << m_Picture.Depth();

  // size of CMAP is 3 bytes * ncolors
  error = PushChunk(iff, NULL, ID_CMAP, 3 * ncolors);
  if (error != 0)
  {
    return error;
  }

  pTableEntry = m_Picture.GetColors32();
  for (; ncolors; --ncolors)  // at loop start ncolors has its last value
  {
    cmapReg.red = *pTableEntry;
    pTableEntry++;
    cmapReg.green = *pTableEntry;
    pTableEntry++;
    cmapReg.blue = *pTableEntry;
    pTableEntry++;

    if ((WriteChunkBytes(iff, (BYTE*)&cmapReg, 3)) != 3)
    {
      return IFFERR_WRITE;
    }
  }

  error = PopChunk(iff);
  return error;
}
