#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include <exec/memory.h>

#include <stdio.h>
#include <string.h>

#include "OpenAmosAbk.h"

OpenAmosAbk::OpenAmosAbk(const char* pFileName)
  : m_pFileBuf(NULL),
    m_FileHandle(Open(pFileName, MODE_OLDFILE)),
    m_FileBufByteSize(0),
    m_ParseByteCounter(0),
    m_NumAbkFrames(0),
    m_AbkFrameId(0),
    m_SheetFramesWordWidth(0),
    m_SheetFramesHeight(0),
    m_SheetFramesDepth(0),
    m_pSheetFramesBitMap(NULL)
{
  if(m_FileHandle == 0)
  {
    cleanup();
    throw "OpenAmosAbk: Failed to open file.";
  }

  // Get the file size
  Seek(m_FileHandle, 0, OFFSET_END);
  m_FileBufByteSize = Seek(m_FileHandle, 0, OFFSET_BEGINING);

  if(m_FileBufByteSize < 6)
  {
    throw "OpenAmosAbk: File is to small to contain a header.";
  }

  // Allocate memory for the whole file
  m_pFileBuf = (BYTE*)AllocVec(m_FileBufByteSize, MEMF_PUBLIC);
  if(m_pFileBuf == NULL)
  {
    cleanup();
    throw "OpenAmosAbk: Failed to allocate memory to read the file.";
  }

  // Read the whole file
  if(Read(m_FileHandle, m_pFileBuf, m_FileBufByteSize) != m_FileBufByteSize)
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

    m_ParseByteCounter++;
  }

  m_NumAbkFrames = readNextWord();
  if(m_NumAbkFrames < 1)
  {
    throw "OpenAmosAbk: Found no pictures in AMOS sprite bank.";
  }
}


OpenAmosAbk::~OpenAmosAbk()
{
  cleanup();
}

struct BitMap* OpenAmosAbk::parseNextAnimSheet()
{
  std::vector<struct BitMap*> sheetBitMaps;

  try
  {

    for(size_t i = m_AbkFrameId; i < m_NumAbkFrames; i++)
    {
      ULONG wordWidth = readNextWord();
      ULONG height = readNextWord();
      ULONG depth = readNextWord();
      readNextWord();  // skip hotSpotX
      readNextWord();  // skip hotSpotY

      if(m_SheetFramesWordWidth == 0 
      && m_SheetFramesHeight == 0 
      && m_SheetFramesDepth == 0)
      {
        // Start of a new sheet
        m_SheetFramesWordWidth = wordWidth;
        m_SheetFramesHeight = height;
        m_SheetFramesDepth = depth;
        
        if(m_pSheetFramesBitMap != NULL)
        {
          FreeBitMap(m_pSheetFramesBitMap);
          m_pSheetFramesBitMap = NULL;
        }
      }
      else
      {
        if((depth != m_SheetFramesDepth)
        || (height != m_SheetFramesHeight)
        || (wordWidth != m_SheetFramesWordWidth))
        {
          // This image has an other dimension than the current script.
          break;
        }
      }

      m_AbkFrameId++;
    }

    // Finsih the current sheet.

    // TODO
    //createSheetFramesBitMap();
    //clearSheetBitMapVector();

    // Mark start of a new sheet
    m_SheetFramesWordWidth = 0;
    m_SheetFramesHeight = 0;
    m_SheetFramesDepth = 0;

    // Rewind the counter by 10 Bytes (the 5 x readNextWord()
    // calls above). These reads must be re-done when the next,
    // new sheet is parsed.
    m_ParseByteCounter -= 10;

    return m_pSheetFramesBitMap;
 
    // printf("File contains %d pictures\n", m_NumAbkFrames);

    // printf("The first one has a size of %d x %d\n", wordWidth, height);
    
  }
  catch(const char* pErrMsg)
  {
    return NULL;
  }
 

}

ULONG* OpenAmosAbk::parseColors32()
{

  return NULL;
}

struct BitMap* OpenAmosAbk::createCurrentFrameBitMap()
{
  ULONG planeSize = m_SheetFramesWordWidth * 2 * m_SheetFramesHeight;
  ULONG allPlanesSize = planeSize * m_SheetFramesDepth;

  struct BitMap* pFrameBitMap = AllocBitMap(m_SheetFramesWordWidth * 16,
                                            m_SheetFramesHeight,
                                            m_SheetFramesDepth, 
                                            BMF_CLEAR,
                                            NULL);
  if(pFrameBitMap == NULL)
  {
    throw "OpenAmosAbk: failed to allocate memory for SheetFrames BitMap.";
  }

  struct BitMap abkBitmap;
  InitBitMap(&abkBitmap, 
             m_SheetFramesDepth, 
             m_SheetFramesWordWidth * 2, 
             m_SheetFramesHeight);

  // Manually set all plane pointers to the dedicated area of 
  // destination Bitmap
  PLANEPTR ptr = (PLANEPTR)(m_pFileBuf + m_ParseByteCounter);
  for(size_t i = 0; i < m_SheetFramesDepth; i++)
  {
    abkBitmap.Planes[i] = ptr;
    ptr += planeSize;
  }

  // Blit abk Bitmap to destination BitMap
  BltBitMap(&abkBitmap, 
            0,
            0,
            pFrameBitMap,
            0,
            0,
            m_SheetFramesWordWidth * 16,
            m_SheetFramesHeight,
            0xC0, 
            0xFF, 
            NULL);

  // Don't forget to forward the parse counter by the amount of bytes
  // processed
  m_ParseByteCounter += allPlanesSize;

  return pFrameBitMap;
}


bool OpenAmosAbk::createSheetFramesBitmap(const std::vector<struct BitMap*>& frames)
{
  return false;
}

ULONG OpenAmosAbk::readNextWord()
{
  ULONG value;

  if((m_ParseByteCounter + 2) >= m_FileBufByteSize)
  {
    throw "OpenAmosAbk::readNextWord() reached end of buffer.";  
  }

  value = 256 * m_pFileBuf[m_ParseByteCounter] 
        + m_pFileBuf[m_ParseByteCounter + 1];
  
  m_ParseByteCounter += 2;
  return value;
}


void OpenAmosAbk::cleanup()
{
  if(m_pSheetFramesBitMap != NULL)
  {
    FreeBitMap(m_pSheetFramesBitMap);
    m_pSheetFramesBitMap = NULL;
  }

  if(m_FileHandle != 0)
  {
    Close(m_FileHandle);
    m_FileHandle = 0;
  }
}
