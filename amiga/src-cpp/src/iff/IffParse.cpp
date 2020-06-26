#include <clib/dos_protos.h>
#include <clib/iffparse_protos.h>
#include <dos/dos.h>

#include "IffParse.h"

IffParse::IffParse(const char* pFileName)
  : m_pIffHandle(NULL)
{
  if (pFileName == NULL)
  {
    throw "IffParser: No file name provided.";
  }

  m_pIffHandle = AllocIFF();
  if(m_pIffHandle == NULL)
  {
    throw "IffParser: Failed to AllocIFF.";
  }

  m_pIffHandle->iff_Stream = Open(pFileName, MODE_OLDFILE);
  if (m_pIffHandle->iff_Stream == 0)
  {
    throw "IffParser: Failed to open file.";
  }

  InitIFFasDOS(m_pIffHandle);
}

IffParse::~IffParse()
{
  if(m_pIffHandle != NULL)
  {
    if(m_pIffHandle->iff_Stream != 0)
    {
      Close(m_pIffHandle->iff_Stream);
    }

    FreeIFF(m_pIffHandle);
    m_pIffHandle = NULL;
  }
}

struct IFFHandle* IffParse::Handle()
{
  return m_pIffHandle;
}

LONG IffParse::currentChunkIs(LONG type, LONG id) const
{
  struct ContextNode* pContextNode;
  LONG result = 0;

  if ((pContextNode = CurrentChunk(m_pIffHandle)) != NULL)
  {
    if ((pContextNode->cn_Type == type) && (pContextNode->cn_ID == id))
    {
      result = 1;
    }
  }

  return result;
}
