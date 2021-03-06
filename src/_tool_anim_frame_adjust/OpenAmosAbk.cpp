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
    m_pColors32(NULL),
    m_FileBufByteSize(0),
    m_ParseByteCounter(0),
    m_NumAbkFrames(0),
    m_AbkFrameId(0),
    m_SheetFramesWordWidth(0),
    m_SheetFramesHeight(0),
    m_SheetFramesDepth(0),
    m_LastParsedWordWidth(0),
    m_pSheetBitMap(NULL)
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
  m_pFileBuf = (UBYTE*)AllocVec(m_FileBufByteSize, MEMF_PUBLIC|MEMF_CHIP);
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
      cleanup();
      throw "OpenAmosAbk: File is no AMOS sprite bank.";
    }

    m_ParseByteCounter++;
  }

  m_NumAbkFrames = readNextWord();
  if(m_NumAbkFrames < 1)
  {
    cleanup();
    throw "OpenAmosAbk: Found no pictures in AMOS sprite bank.";
  }
}


OpenAmosAbk::~OpenAmosAbk()
{
  cleanup();
}

struct BitMap* OpenAmosAbk::parseNextAnimSheet()
{
  std::vector<struct BitMap*> frameVec;

  if(m_AbkFrameId == m_NumAbkFrames)
  {
    // Already read all frames
    return NULL;
  }

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

        if(m_pSheetBitMap != NULL)
        {
          FreeBitMap(m_pSheetBitMap);
          m_pSheetBitMap = NULL;
        }
      }
      else
      {
        if((depth != m_SheetFramesDepth)
        || (height != m_SheetFramesHeight)
        || (wordWidth != m_SheetFramesWordWidth))
        {
          // This image has an other dimension than the current script.

          // Rewind the counter by 10 Bytes (the 5 x readNextWord()
          // calls above). These reads must be re-done when the next,
          // new sheet is parsed.
          m_ParseByteCounter -= 10;
          break;
        }
      }

      struct BitMap* pFrameBitMap = createFrameBitMap();
      if(pFrameBitMap == NULL)
      {
        // Failed to create a BitMap for a abk frame
        return NULL;
      }

      frameVec.push_back(pFrameBitMap);
      m_AbkFrameId++;
    }

    //
    // Finsih the current sheet.
    //

    // First create the resulting BitMap which contains all frames of
    // this sheet.
    if(createSheetBitMap(frameVec) == false)
    {
      return NULL;
    }

    // Now clean up
    clearBitMapVector(frameVec);

    // Mark start of a new sheet
    m_LastParsedWordWidth = m_SheetFramesWordWidth;
    m_SheetFramesWordWidth = 0;
    m_SheetFramesHeight = 0;
    m_SheetFramesDepth = 0;

    return m_pSheetBitMap;
  }
  catch(const char* pErrMsg)
  {
    printf("Exception:\n%s\n", pErrMsg);
    return NULL;
  }


}

ULONG OpenAmosAbk::getSheetWordWidth()
{
  return m_LastParsedWordWidth;
}


ULONG* OpenAmosAbk::parseColors32()
{
  if(m_pColors32 != NULL)
  {
    // Already done
    return m_pColors32;
  }

  // Parse the 32 colors of the AMOS abk
  const ULONG numColors = 32;
  m_pColors32 = (ULONG*) AllocVec((2 + 3 * numColors) * sizeof(ULONG),
                                  MEMF_PUBLIC|MEMF_CLEAR);
  if(m_pColors32 == NULL)
  {
    return NULL;
  }

  // Header item: Write number of colors in the left word of
  // m_pCOlors[0] (The right word is left at '0' so that LoadRgb32()
  // starts loading from Color00)
  m_pColors32[0] = numColors << 16;

  // To fill the colors use another pointer and skip the header item
  ULONG* pCol = m_pColors32 + 1;

  for(size_t i = 0; i < numColors; i++)
  {
    ULONG colorWord = readNextWord();

    ULONG red = (colorWord & 0xf00) >> 8;
    ULONG green = (colorWord & 0xf0) >> 4;
    ULONG blue = (colorWord & 0xf);

    // Convert this 4-bit-value to an 8-bit-value using the formula
    //   cNew = cOld * 16 + 4
    // This will give the these values:
    //
    //  cOld | cNew
    //  -----|-----
    //     0 |   0
    //     1 |  17
    //     2 |  34
    //
    //      ...
    //
    //    15 | 255

    red = (red << 4) + red;
    green = (green << 4) + green;
    blue = (blue << 4) + blue;

    // Calculate the indeces in color table
    size_t iRed = (i + 1) * 3 - 3;
    size_t iGreen = (i + 1) * 3 - 2;
    size_t iBlue = (i + 1) * 3 - 1;

    // Write the colors to the Colors32 table
    pCol[iRed] = red | (red << 8) | (red << 16) | (red << 24);
    pCol[iGreen] = green | (green << 8) | (green << 16) | (green << 24);
    pCol[iBlue] = blue | (blue << 8) | (blue << 16) | (blue << 24);
  }

  return m_pColors32;
}


ULONG OpenAmosAbk::readNextWord()
{
  ULONG value;

  if((m_ParseByteCounter + 1) >= m_FileBufByteSize)
  {
    throw "OpenAmosAbk::readNextWord() reached end of buffer.";
  }

  value = (m_pFileBuf[m_ParseByteCounter++] << 8);
  value |= m_pFileBuf[m_ParseByteCounter++];

  return value;
}

struct BitMap* OpenAmosAbk::createFrameBitMap()
{
  /**
   * NOTE: Theoritically!! this method could be changed. It would
   * suffice to create the abkBitmap dynamically (AllocVec! not
   * AllocBitmap), then do InitBitMap to it, set the ptr's in the for
   * loop and return that BitMap. No pFrameBitMap allocation and no
   * blitting to it would be needed.
   *
   * But practically that wouldn't work: The BitMap here is not freed
   * after use from within this class. It has to be freed from outside
   * with FreeBitMap() and that would crash if the BitMap would've been
   * created with AllocVec().
   */

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
             m_SheetFramesWordWidth * 16,
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


bool OpenAmosAbk::createSheetBitMap(std::vector<struct BitMap*>& frameVec)
{
  ULONG frameWidth = m_SheetFramesWordWidth * 16;
  ULONG width = frameWidth * frameVec.size();

  if(m_pSheetBitMap != NULL)
  {
    FreeBitMap(m_pSheetBitMap);
  }

  m_pSheetBitMap = AllocBitMap(width,
                               m_SheetFramesHeight,
                               m_SheetFramesDepth,
                               BMF_CLEAR,
                               NULL);

  if(m_pSheetBitMap == NULL)
  {
    return false;
  }

  ULONG xStart = 0;
  for(std::vector<struct BitMap*>::iterator it = frameVec.begin(); it != frameVec.end(); ++it)
  {
    struct BitMap* pSrcBitMap = *it;

    // Blit given portion of the source Bitmap to destination BitMap
    BltBitMap(pSrcBitMap,
              0,
              0,
              m_pSheetBitMap,
              xStart,
              0,
              frameWidth,
              m_SheetFramesHeight,
              0xC0,
              0xFF,
              NULL);

    xStart += frameWidth;
  }

  return true;
}


void OpenAmosAbk::clearBitMapVector(std::vector<struct BitMap*>& v)
{
  for(std::vector<struct BitMap*>::iterator it = v.begin(); it != v.end(); ++it)
  {
    if((*it) != NULL)
    {
      FreeBitMap(*it);
    }
  }

  v.clear();
}

void OpenAmosAbk::cleanup()
{
  if(m_pColors32 != NULL)
  {
    FreeVec(m_pColors32);
    m_pColors32 = NULL;
  }

  if(m_pSheetBitMap != NULL)
  {
    FreeBitMap(m_pSheetBitMap);
    m_pSheetBitMap = NULL;
  }

  if(m_pFileBuf != NULL)
  {
    FreeVec(m_pFileBuf);
    m_pFileBuf = NULL;
  }


  if(m_FileHandle != 0)
  {
    Close(m_FileHandle);
    m_FileHandle = 0;
  }
}
