#include <clib/dos_protos.h>

#include "OpenAmosAbk.h"

OpenAmosAbk::OpenAmosAbk(const char* pFileName)
  : m_FileHandle(Open(pFileName, MODE_OLDFILE))
{
  if(m_FileHandle == 0)
  {
    throw "Failed to open given file.";
  }

  //Read();

}


OpenAmosAbk::~OpenAmosAbk()
{
  cleanup();
}

struct BitMap* OpenAmosAbk::parseNextAnimSheet()
{
  return NULL;
}

ULONG* OpenAmosAbk::parseColors32()
{

  return NULL;
}


void OpenAmosAbk::cleanup()
{
  if(m_FileHandle != 0)
  {
    Close(m_FileHandle);
    m_FileHandle = 0;
  }
}
