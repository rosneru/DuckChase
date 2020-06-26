#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/iffparse_protos.h>
#include <datatypes/soundclass.h> // Not using  datatypes, just for
                                  // 'BitMapHeader', 'ID_xyz', etc.
#include "IffParse.h"
#include "SoundFile8SVX.h"

#define MAXOCT 16

/* 8SVX Property chunks to be grabbed
 */
LONG esvxprops[] = {ID_8SVX, ID_VHDR, ID_8SVX, ID_NAME, TAG_DONE};

Soundfile8SVX::Soundfile8SVX(const char* pFileName)
  : m_pVoice8Header(NULL)
{
  IffParse iffParse(pFileName);

  // Define which chunks to load
  PropChunk(iffParse.Handle(), ID_8SVX, ID_VHDR);
  StopChunk(iffParse.Handle(), ID_8SVX, ID_BODY);

  // Parse the iff file
  LONG iffErr = ParseIFF(iffParse.Handle(), IFFPARSE_SCAN);
  if(iffErr != 0)
  {
    throw "Soundfile8SVX: Error in ParseIFF.";
  }

  // Load VHDR (Voice8Header)
  StoredProperty* pStoredProp = FindProp(iffParse.Handle(), ID_8SVX, ID_VHDR);
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
}

Soundfile8SVX::~Soundfile8SVX()
{

}


bool Soundfile8SVX::decode8SVXBody()
{
  return false;
}