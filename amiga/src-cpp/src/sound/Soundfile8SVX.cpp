#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/iffparse_protos.h>
#include <datatypes/soundclass.h> // Not using  datatypes, just for
                                  // 'BitMapHeader', 'ID_xyz', etc.

#include "SoundFile8SVX.h"

#define MAXOCT 16

/* 8SVX Property chunks to be grabbed
 */
LONG esvxprops[] = {ID_8SVX, ID_VHDR, ID_8SVX, ID_NAME, TAG_DONE};

Soundfile8SVX::Soundfile8SVX(const char* pFileName)
  : m_pIffHandle(NULL),
    m_pVoice8Header(NULL)
{
  if (pFileName == NULL)
  {
    throw "Soundfile8SVX: No file name provided.";
  }

  m_pIffHandle = AllocIFF();
  if(m_pIffHandle == NULL)
  {
    throw "Soundfile8SVX: Failed to AllocIFF.";
  }

  m_pIffHandle->iff_Stream = Open(pFileName, MODE_OLDFILE);
  if (m_pIffHandle->iff_Stream == 0)
  {
    throw "Soundfile8SVX: Failed to open file.";
  }

  InitIFFasDOS(m_pIffHandle);

  LONG iffErr;
  if((iffErr = OpenIFF(m_pIffHandle, IFFF_READ)) != 0)
  {
    throw "Soundfile8SVX: OpenIFF returned error.";
  }

  // Define which chunks to load
  PropChunk(m_pIffHandle, ID_8SVX, ID_VHDR);
  StopChunk(m_pIffHandle, ID_8SVX, ID_BODY);

  // Parse the iff file
  iffErr = ParseIFF(m_pIffHandle, IFFPARSE_SCAN);
  if(iffErr != 0)
  {
    throw "Soundfile8SVX: Error in ParseIFF.";
  }

  // Load VHDR (Voice8Header)
  StoredProperty* pStoredProp = FindProp(m_pIffHandle, ID_8SVX, ID_VHDR);
  if(pStoredProp == NULL)
  {
    throw "Soundfile8SVX: No Voice8Header found in file.";
  }

  m_pVoice8Header = (struct Voice8Header*)pStoredProp->sp_Data;
  if(m_pVoice8Header == NULL)
  {
    throw "Soundfile8SVX: Voice8Header of sound file is empty.";
  }

  if(decode8SVXBody() == false)
  {
    throw "Soundfile8SVX: Error while decoding the 8SVX body.";
  }

  Close(m_pIffHandle->iff_Stream);
  m_pIffHandle->iff_Stream = 0;

  CloseIFF(m_pIffHandle);
  FreeIFF(m_pIffHandle);
  m_pIffHandle = NULL;
}

Soundfile8SVX::~Soundfile8SVX()
{
  if(m_pIffHandle != NULL)
  {
    if(m_pIffHandle->iff_Stream != 0)
    {
      Close(m_pIffHandle->iff_Stream);
    }

    CloseIFF(m_pIffHandle);
    FreeIFF(m_pIffHandle);
    m_pIffHandle = NULL;
  }
}


bool Soundfile8SVX::decode8SVXBody()
{
  return false;
}