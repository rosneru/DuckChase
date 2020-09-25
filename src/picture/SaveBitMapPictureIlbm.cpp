#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/iffparse_protos.h>

#include <exec/memory.h>
#include <graphics/view.h>
#include <libraries/iffparse.h>

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
    m_MAX_SAVE_DEPTH(24),
    m_ModeId(picture.GetModeId()),
    m_Bmhd(),
    bodybuf(NULL)
{
  if (!(m_ModeId & 0xFFFF0000))
  {
    m_ModeId &= OLDCAMGMASK;
  }

  if (!(bodybuf = (UBYTE*) AllocVec(m_BODY_BUF_SIZE, MEMF_PUBLIC)))
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

  // Initialize and write the BitMapHeader BMHD
  initBitMapHeader();
  err = iff.PutCk(ID_BMHD, sizeof(struct BitMapHeader), (BYTE*)&m_Bmhd);
  if(err != 0)
  {
    throw "SaveBitMapPictureIlbm: Failed to write BitMapHeader.";
  }

  // Write the ColorMap CMAP
  if(m_Picture.GetColors32() != NULL)
  {
    err = PutCmap(iff);
    if(err != 0)
    {
      throw "SaveBitMapPictureIlbm: Failed to write ColorMap.";
    }
  }

  // Write the screen mode id CAMG
  iff.PutCk(ID_CAMG, sizeof(ULONG),(BYTE*) m_Picture.GetModeId());

  // Write the body
  err = PutBody(iff);
  if(err != 0)
  {
    throw "SaveBitMapPictureIlbm: Failed to write body.";
  }

  // After an object is successfully created the file is already saved
  // and we're done. All resources can be freed immediately.
  cleanup();
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


/* Advisory that 8 significant bits-per-gun have been stored in CMAP
 * i.e. that the CMAP is definitely not 4-bit values shifted left.
 * This bit will disable nibble examination by color loading routine.
 */
#define BMHDB_CMAPOK	7
#define BMHDF_CMAPOK	(1 << BMHDB_CMAPOK)

void SaveBitMapPictureIlbm::initBitMapHeader()
{
  struct DisplayInfo DI;

  m_Bmhd.bmh_Width = m_Picture.Width();
  m_Bmhd.bmh_Height = m_Picture.Height();
  m_Bmhd.bmh_Left = m_Bmhd.bmh_Top = 0;   // Default position is (0,0)
  m_Bmhd.bmh_Depth = m_Picture.Depth();
  m_Bmhd.bmh_Masking = mskHasMask;
  m_Bmhd.bmh_Compression = cmpByteRun1;
  m_Bmhd.bmh_Pad = BMHDF_CMAPOK; // we will store 8 significant bits
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


long SaveBitMapPictureIlbm::PutCmap(IffParse& iff)
{
  long error;
  ULONG* pTableEntry;
  ColorRegister cmapReg;

  pTableEntry = m_Picture.GetColors32();

  // First entry is the number of colors
  ULONG ncolors = *(pTableEntry++) >> 16;

  // size of CMAP is 3 bytes * ncolors
  error = PushChunk(iff.Handle(), NULL, ID_CMAP, 3 * ncolors);
  if (error != 0)
  {
    return error;
  }

  for (; ncolors; --ncolors)  // at loop start ncolors has its last value
  {
    cmapReg.red   = *(pTableEntry++) >> 24;
    cmapReg.green = *(pTableEntry++) >> 24;
    cmapReg.blue  = *(pTableEntry++) >> 24;

    if ((WriteChunkBytes(iff.Handle(), (BYTE*)&cmapReg, 3)) != 3)
    {
      return IFFERR_WRITE;
    }
  }

  error = PopChunk(iff.Handle());
  return error;
}


long SaveBitMapPictureIlbm::PutBody(IffParse& iff)
{
  long numBytes;
  long error;
  LONG rowBytes = m_Picture.GetBitMap()->BytesPerRow;   // for source modulo only
  LONG FileRowBytes = iff.rowBytes(m_Picture.Width());  // width to write in bytes
  ULONG dstDepth = m_Picture.Depth();
  UBYTE compression = m_Bmhd.bmh_Compression;
  ULONG planeCnt;                         // number of bit planes including mask
  ULONG iPlane, iRow;
  LONG packedRowBytes;
  PLANEPTR buf;
  PLANEPTR planes[m_MAX_SAVE_DEPTH + 1];   // array of ptrs to planes & mask

  // TODO FIXME cast
  PLANEPTR mask = const_cast<BitMapPictureBase&>(m_Picture).GetMask()->Planes[0];


  if (m_BODY_BUF_SIZE < iff.maxPackedSize(FileRowBytes) ||  // Must buffer a comprsd row
      compression > cmpByteRun1 ||                          // Not supported
      dstDepth > m_MAX_SAVE_DEPTH)                          // Not supported
  {
    return -1;
  }

  planeCnt = dstDepth + (mask == NULL ? 0 : 1);

  // Copy the ptrs to bit & mask planes into local array "planes"
  for (iPlane = 0; iPlane < dstDepth; iPlane++)
  {
    planes[iPlane] = m_Picture.GetBitMap()->Planes[iPlane];
  }

  if (mask != NULL)
  {
    planes[dstDepth] = mask;
  }

  // Write a BODY chunk header
  error = PushChunk(iff.Handle(), NULL, ID_BODY, IFFSIZE_UNKNOWN);
  if (error != 0)
  {
    return (error);
  }

  // Write the BODY contents
  for (iRow = m_Picture.Height(); iRow > 0; iRow--)
  {
    for (iPlane = 0; iPlane < planeCnt; iPlane++)
    {
      if (compression == cmpNone)
      {
        // Write next row
        numBytes = WriteChunkBytes(iff.Handle(), planes[iPlane], FileRowBytes);
        if (numBytes != FileRowBytes)
        {
          return IFFERR_WRITE;
        }

        planes[iPlane] += rowBytes; // Possibly skipping unused bytes
      }
      else
      {
        // Compress and write next row
        buf = bodybuf;
        packedRowBytes = packrow(&planes[iPlane], &buf, FileRowBytes);

        // Note that packrow incremented planes already by FileRowBytes
        planes[iPlane] += rowBytes - FileRowBytes;  // Possibly skipping unused bytes
        numBytes = WriteChunkBytes(iff.Handle(), bodybuf, packedRowBytes);
        if (numBytes != packedRowBytes)
        {
          return IFFERR_WRITE;
        }
      }
    }
  }

  // Finish the chunk
  error = PopChunk(iff.Handle());
  return error;
}

static LONG putSize;
static char buf[256];   // [TBD] should be 128?  on stack?

#define DUMP 0
#define RUN 1

#define MinRun 3
#define MaxRun 128
#define MaxDat 128

#define OutDump(nn)   dest = PutDump(dest, nn)
#define OutRun(nn,cc) dest = PutRun(dest, nn, cc)



static UBYTE *PutDump(UBYTE *dest, int nn)
{
  int i;

  // Putting the byte nn-1
  *dest++ = (nn-1);
  ++putSize;

  for(i = 0;  i < nn;  i++)
  {
    // Putting the byte buf[i]
    *dest++ = (buf[i]);
    ++putSize;
  }

  return dest;
}


static UBYTE *PutRun(UBYTE *dest, int nn, int cc)
{
  // Putting the byte -(nn-1)
  *dest++ = (-(nn-1));
  ++putSize;

  // Putting the byte cc
  *dest++ = (cc);
  ++putSize;

  return dest;
}


LONG SaveBitMapPictureIlbm::packrow(UBYTE** ppSource,
                                    UBYTE** ppDest,
                                    LONG rowSize)
{
  UBYTE *source, *dest;
  char c, lastc = '\0';
  BOOL mode = DUMP;
  short nbuf = 0;   // number of chars in buffer
  short rstart = 0; // buffer index current run starts

  source = *ppSource;
  dest = *ppDest;
  putSize = 0;

  // (Filling buffer with all bytes?)
  buf[0] = lastc = c = (*source++); // so have valid lastc
  nbuf = 1;
  rowSize--; /* since one byte eaten.*/

  for (; rowSize; --rowSize)
  {
    buf[nbuf++] = c = (*source++);
    switch (mode)
    {
    case DUMP:
      // If the buffer is full, write the length byte, then the data
      if (nbuf > MaxDat)
      {
        OutDump(nbuf - 1);
        buf[0] = c;
        nbuf = 1;
        rstart = 0;
        break;
      }

      if (c == lastc)
      {
        if (nbuf - rstart >= MinRun)
        {
          if (rstart > 0)
          {
            OutDump(rstart);
          }

          mode = RUN;
        }
        else if (rstart == 0)
        {
          // No dump in progress, so can't lose by making these 2 a run.
          mode = RUN;
        }
      }
      else
      {
        // First of run
        rstart = nbuf - 1;
      }
      break;

    case RUN:
      if ((c != lastc) || (nbuf - rstart > MaxRun))
      {
        // output run
        OutRun(nbuf - 1 - rstart, lastc);
        buf[0] = c;
        nbuf = 1;
        rstart = 0;
        mode = DUMP;
      }
      break;
    }

    lastc = c;
  }

  switch (mode)
  {
  case DUMP:
    OutDump(nbuf);
    break;
  case RUN:
    OutRun(nbuf - rstart, lastc);
    break;
  }

  *ppSource = source;
  *ppDest = dest;

  return putSize;
}
