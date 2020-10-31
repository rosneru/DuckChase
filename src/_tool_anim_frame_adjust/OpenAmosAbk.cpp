#include <clib/dos_protos.h>
#include <clib/exec_protos.h>

#include <exec/memory.h>

#include <stdio.h>
#include <string.h>

#include "OpenAmosAbk.h"

OpenAmosAbk::OpenAmosAbk(const char* pFileName)
  : m_pFileBuf(NULL),
    m_FileHandle(Open(pFileName, MODE_OLDFILE))
    
{
  if(m_FileHandle == 0)
  {
    cleanup();
    throw "OpenAmosAbk: Failed to open file.";
  }

  // Get the file size
  Seek(m_FileHandle, 0, OFFSET_END);
  ULONG byteSize = Seek(m_FileHandle, 0, OFFSET_BEGINING);

  if(byteSize < 6)
  {
    throw "OpenAmosAbk: File is to small to contain a header.";
  }

  // Allocate memory for the whole file
  m_pFileBuf = (BYTE*)AllocVec(byteSize, MEMF_PUBLIC);
  if(m_pFileBuf == NULL)
  {
    cleanup();
    throw "OpenAmosAbk: Failed to allocate memory to read the file.";
  }

  // Read the whole file
  if(Read(m_FileHandle, m_pFileBuf, byteSize) != byteSize)
  {
    cleanup();
    throw "OpenAmosAbk: Failed to read file identifier.";
  }

  Close(m_FileHandle);
  m_FileHandle = 0;

  BYTE identifier[] = {'A', 'm', 'S', 'p'};
  for(ULONG i = 0; i < sizeof(identifier) / sizeof(identifier[0]); i++)
  {
    if(m_pFileBuf[i] != identifier[i])
    {
      throw "OpenAmosAbk: File is no AMOS sprite bank.";
    }
  }

  ULONG numPictures = 255 * m_pFileBuf[4] + m_pFileBuf[5];
  ULONG w = 255 * m_pFileBuf[6] + m_pFileBuf[7];
  ULONG h = 255 * m_pFileBuf[8] + m_pFileBuf[9];

  printf("File contains %d pictures\n", numPictures);

  printf("The first one has a size of %d x %d\n", w, h);
  
  throw "Not ready yet.";

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
