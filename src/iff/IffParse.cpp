#include <clib/dos_protos.h>
#include <clib/iffparse_protos.h>
#include <dos/dos.h>

#include "IffParse.h"

IffParse::IffParse(const char* pFileName, bool isWriteMode)
  : m_pIffHandle(NULL),
    m_IsIffOpen(true)
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

  LONG openFileMode = isWriteMode ? MODE_NEWFILE : MODE_OLDFILE;
  m_pIffHandle->iff_Stream = Open(pFileName, openFileMode);
  if (m_pIffHandle->iff_Stream == 0)
  {
    cleanup();
    throw "IffParser: Failed to open file.";
  }

  InitIFFasDOS(m_pIffHandle);

  LONG openIffMode = isWriteMode ? IFFF_WRITE : IFFF_READ;
  if(OpenIFF(m_pIffHandle, openIffMode) != 0)
  {
    cleanup();
    throw "IffParse: OpenIFF returned error.";
  }

  m_IsIffOpen = true;
}

IffParse::~IffParse()
{
  cleanup();
}

void IffParse::cleanup()
{
  if(m_IsIffOpen)
  {
    CloseIFF(m_pIffHandle);
    m_IsIffOpen = false;
  }

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


long IffParse::PutCk(long id, long size, void *data)
{
  long error, wlen;

  error = PushChunk(m_pIffHandle, 0, id, size);
  if(error != 0)
  {
    return error;
  }

  wlen = WriteChunkBytes(m_pIffHandle, data, size);
  if(wlen != size)
  {
    return IFFERR_WRITE;
  }

  error = PopChunk(m_pIffHandle);
  return error;
}
