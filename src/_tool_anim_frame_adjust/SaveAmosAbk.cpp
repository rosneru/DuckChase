#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <exec/memory.h>

#include "SaveAmosAbk.h"


SaveAmosAbk::SaveAmosAbk(const char* pFileName,
                         std::vector<SheetItemNode*> sheets,
                         ULONG* pColors32)
  : m_FileHandle(Open(pFileName, MODE_NEWFILE)),
    m_pOCSColorTable(NULL)
{
  // Check if file opened sucessfully
  if(m_FileHandle == 0)
  {
    cleanup();
    throw "SaveAmosAbk: Failed to open file for writing.";
  }

  // Write 'AmSp'
  if(Write(m_FileHandle, (APTR)"AmSp", 4) != 4)
  {
    cleanup();
    throw "SaveAmosAbk: Wrong size of bytes written.";
  }

  // Calculate the total number of frames
  ULONG numFrames = 0;
  for(std::vector<SheetItemNode*>::iterator it = sheets.begin(); it != sheets.end(); ++it)
  {
    if((*it) != NULL)
    {
      ULONG sheetNumFrames = (*it)->SheetWidth / ((*it)->FrameWordWidth * 16);
      numFrames += sheetNumFrames;
    }
  }

  // Write the total number of frames
  if(writeWord(numFrames) == false)
  {
    throw "SaveAmosAbk: Wrong size of bytes written.";
  }

  // Write all the frames pictures
  for(std::vector<SheetItemNode*>::iterator it = sheets.begin(); it != sheets.end(); ++it)
  {
    if((*it) != NULL)
    {
      // write the frame
    }
  }

  // Create and write the OCS color table
  m_pOCSColorTable = colors32ToOCSColorTable(pColors32);
  
}

SaveAmosAbk::~SaveAmosAbk()
{
  cleanup();
}


void SaveAmosAbk::cleanup()
{
  if(m_pOCSColorTable != NULL)
  {
    FreeVec(m_pOCSColorTable);
    m_pOCSColorTable = NULL;
  }

  if(m_FileHandle != 0)
  {
    Close(m_FileHandle);
    m_FileHandle = 0;
  }
}


ULONG* SaveAmosAbk::colors32ToOCSColorTable(ULONG* pColors32)
{
  ULONG i, red, green, blue;
  ULONG* pCol;
  ULONG colorWord, iRed, iGreen, iBlue;
  ULONG numColors;
  ULONG* pColorTable;

  if(pColors32 == NULL)
  {
    return NULL;
  }

  numColors = (pColors32[0] & 0xffff0000) >> 16;
  if(numColors != 32)
  {
    /* Invalid; OCS color table must have 32 entries */
    return NULL;
  }

  pColorTable = (ULONG*) AllocVec(numColors * sizeof(ULONG), 
                                  MEMF_PUBLIC|MEMF_CLEAR);
  if(pColorTable == NULL)
  {
    return NULL;
  }

  /* Use another pointer to skip Colors32 header item */
  pCol = pColors32 + 1;

  for(i = 0; i < numColors; i++)
  {
    iRed = (i + 1) * 3 - 3;
    iGreen = (i + 1) * 3 - 2;
    iBlue = (i + 1) * 3 - 1;

    red = ((pCol[iRed] & 0xff000000) >> 24) & 0xf;
    green = ((pCol[iGreen] & 0xff000000) >> 24) & 0xf;
    blue = ((pCol[iBlue] & 0xff000000) >> 24) & 0xf;

    colorWord = (red << 8) | (green << 4) | blue;

    pColorTable[i] = colorWord;
  }


  return pColorTable;
}


bool SaveAmosAbk::writeWord(ULONG value)
{
  UBYTE buf[2];
  buf[0] = (value & 0xff00) >> 8;
  buf[1] = value & 0xff;

  return Write(m_FileHandle, &buf, 2) == 2;
}
