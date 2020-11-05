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



  // Write number of frames
  ULONG numFrames = sheets.size();
  ULONG numFrameDigits = numDigits(numFrames);

  for(std::vector<SheetItemNode*>::iterator it = sheets.begin(); it != sheets.end(); ++it)
  {
    if((*it) != NULL)
    {
      // write the frame
    }
  }

  // Write the colors32
  m_pOCSColorTable = colors32ToOCSColorTable(pColors32);
  
}

SaveAmosAbk::~SaveAmosAbk()
{
  cleanup();
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



ULONG SaveAmosAbk::numDigits(ULONG number)
{
  ULONG digits = 1;
  if ( number >= 100000000 ) { digits += 8; number /= 100000000; }
  if ( number >= 10000     ) { digits += 4; number /= 10000; }
  if ( number >= 100       ) { digits += 2; number /= 100; }
  if ( number >= 10        ) { digits += 1; }

  return digits;
}