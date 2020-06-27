#include <stddef.h>

#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/iffparse_protos.h>
#include <datatypes/soundclass.h> // Not using  datatypes, just for
                                  // 'BitMapHeader', 'ID_xyz', etc.
#include <exec/memory.h>

#include "SoundFile8SVX.h"

#define MAXOCT 16

/* 8SVX Property chunks to be grabbed
 */
LONG esvxprops[] = {ID_8SVX, ID_VHDR, ID_8SVX, ID_NAME, TAG_DONE};

Soundfile8SVX::Soundfile8SVX(const char* pFileName)
  : m_pSampleData(NULL),
    m_NumSampleBytes(0),
    m_SamplesPerSec(0),
    m_NumOctaves(0)
{
  IffParse iffParse(pFileName);

  LONG iffErr;
  if((iffErr = OpenIFF(iffParse.Handle(), IFFF_READ)) != 0)
  {
    throw "Soundfile8SVX: OpenIFF returned error.";
  }

  // Define which chunks to load
  PropChunk(iffParse.Handle(), ID_8SVX, ID_VHDR);
  StopChunk(iffParse.Handle(), ID_8SVX, ID_BODY);

  // Parse the iff file
  iffErr = ParseIFF(iffParse.Handle(), IFFPARSE_SCAN);
  if(iffErr != 0)
  {
    CloseIFF(iffParse.Handle());
    throw "Soundfile8SVX: Error in ParseIFF.";
  }

  // Find the sample data header (VHDR / Voice8Header)
  StoredProperty* pStoredProp = FindProp(iffParse.Handle(), ID_8SVX, ID_VHDR);
  if(pStoredProp == NULL)
  {
    CloseIFF(iffParse.Handle());
    throw "Soundfile8SVX: No Voice8Header found in file.";
  }

  // Get the sample data header
  struct Voice8Header* pVoice8Hdr = (struct Voice8Header*)pStoredProp->sp_Data;
  if(pVoice8Hdr == NULL)
  {
    CloseIFF(iffParse.Handle());
    throw "Soundfile8SVX: Voice8Header of sound file is empty.";
  }

  // Copying some values we may need later when sample data header 
  // isn't  valid anymore
  m_SamplesPerSec = pVoice8Hdr->samplesPerSec;
  m_NumOctaves = pVoice8Hdr->ctOctave;

  if(decode8SVXBody(iffParse, pVoice8Hdr) == false)
  {
    CloseIFF(iffParse.Handle());
    throw "Soundfile8SVX: Error while decoding the 8SVX body.";
  }

  UWORD osize = pVoice8Hdr->oneShotHiSamples;
  UWORD rsize = pVoice8Hdr->repeatHiSamples;
  UWORD spcyc = pVoice8Hdr->samplesPerHiCycle;
  if (!spcyc)
  {
    spcyc = pVoice8Hdr->repeatHiSamples;
  }

  if (!spcyc)
  {
    spcyc = 8;
  }

  BYTE* oneshot = m_pSampleData;
  for(size_t oct = m_NumOctaves - 1; oct >= 0; oct--)
  {

    oneshot += (osize + rsize);
    osize <<= 1; 
    rsize <<= 1; 
    spcyc <<= 1;
  }

  CloseIFF(iffParse.Handle());
}


Soundfile8SVX::~Soundfile8SVX()
{
  if(m_pSampleData != NULL)
  {
    FreeVec(m_pSampleData);
    m_pSampleData = NULL;
  }
}


UWORD Soundfile8SVX::SamplesPerSec() const
{
  return m_SamplesPerSec;
}


UBYTE Soundfile8SVX::NumOctaves() const
{
  return m_NumOctaves;
}


bool Soundfile8SVX::decode8SVXBody(IffParse& iffParse, 
                                   struct Voice8Header* pHdr)
{
  if(iffParse.currentChunkIs(ID_8SVX, ID_BODY) == false)
  {
    // No body chunk
    return false;
  }

  struct ContextNode* pContextNode = CurrentChunk(iffParse.Handle());
  LONG numSampleBytes = iffParse.chunkMoreBytes(pContextNode);

  // If we have to decompress, let's just load it into public mem
  ULONG memtype = pHdr->sCompression ? MEMF_PUBLIC : MEMF_CHIP;
  m_pSampleData = (BYTE*) AllocVec(numSampleBytes, memtype);
  if(m_pSampleData == NULL)
  {
    // Failed to allocate memory for reading the 8svx body into
    return false;
  }

  m_NumSampleBytes = numSampleBytes;
  LONG rlen = ReadChunkBytes(iffParse.Handle(), m_pSampleData, numSampleBytes);
  if(rlen != numSampleBytes)
  {
    // Stream read error
    return false;
  }

  if(pHdr->sCompression == sCmpFibDelta)
  {
    BYTE* pDecompressedSampleData = (BYTE*) AllocVec(numSampleBytes << 1, 
                                                     MEMF_CHIP);
    if(pDecompressedSampleData == NULL)
    {
      // Failed to allocate memory for uncompressing the 8svx body into
      return false;
    }

    DUnpack(m_pSampleData, numSampleBytes, pDecompressedSampleData);
    FreeVec(m_pSampleData);
    m_pSampleData = pDecompressedSampleData;
    m_NumSampleBytes = numSampleBytes << 1;
  }
  else if(pHdr->sCompression != sCmpNone)
  {
    // Unknown compression method
    return false;
  }
  

  return true;
}


void Soundfile8SVX::DUnpack(BYTE source[], LONG n, BYTE dest[])
{
  /**
   * ABOUT #1
   * RKRM 'Devices': This is Steve Hayes' Fibonacci Delta sound
   * compression technique.  It's like the traditional delta encoding
   * but encodes each delta in a mere 4 bits.  The compressed data is
   * half the size of the original data plus a 2-byte overhead for the
   * initial value.  This much compression introduces some distortion,
   * so try it out and use it with discretion.
   */

  /**
   * ABOUT #2
   * The algorithm was first described by Steve Hayes and was used in
   * 8SVX audio stored in the Interchange File Format (IFF). The quality
   * loss is considerable (especially when the audio contained many
   * large deltas) and was even in the time it was developed (1985) not
   * used much.
   */

  D1Unpack(source + 2, n - 2, dest, source[1]);
}

BYTE codeToDelta[16] = {-34, -21, -13, -8, -5, -3, -2, -1,
                        0,   1,   2,   3,  5,  8,  13, 21};

BYTE Soundfile8SVX::D1Unpack(BYTE source[], LONG n, BYTE dest[], BYTE x)
{
  BYTE d;
  LONG i, lim;

  lim = n << 1;
  for (i = 0; i < lim; ++i)
  {
    /* Decode a data nibble, high nibble then low nibble */
    d = source[i >> 1]; /* get a pair of nibbles */
    if (i & 1)          /* select low or high nibble */
      d &= 0xf;         /* mask to get the low nibble */
    else
      d >>= 4;           /* shift to get the high nibble */
    x += codeToDelta[d]; /* add in the decoded delta */
    dest[i] = x;         /* store a 1 byte sample */
  }
  return (x);
}
