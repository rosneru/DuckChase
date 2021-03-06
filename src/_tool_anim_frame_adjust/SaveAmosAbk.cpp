#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
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
  writeWord(numFrames);

  // Write all the frames pictures
  for(std::vector<SheetItemNode*>::iterator it = sheets.begin(); it != sheets.end(); ++it)
  {
    if((*it) != NULL)
    {
      ULONG frameWidth = ((*it)->FrameWordWidth * 16);
      ULONG sheetNumFrames = (*it)->SheetWidth / frameWidth;
      for(ULONG xStart = 0; xStart < sheetNumFrames * frameWidth; xStart += frameWidth)
      {
        writeWord((*it)->FrameWordWidth);
        writeWord((*it)->SheetHeight);
        writeWord((*it)->SheetDepth);
        writeWord(0); // Write 0 for x hot-spot
        writeWord(0); // Write 0 for y hot-spot

        // ImagedataSize is needed to determine how many bytes to write;
        // Will be set in sheetBitmapToFrameImage()
        ULONG imgDataSize;

        // Convert frame BitMap into ImageData where all planes are
        // located one after another.
        WORD* pImageData = sheetBitmapToFrameImage((*it)->pBitMap, xStart, sheetNumFrames, imgDataSize);
        if(pImageData == NULL)
        {
          cleanup();
          throw "SaveAmosAbk: Failed to create ImageData to save.";
        }

        // Write the frame ImageData
        if(Write(m_FileHandle, (APTR)pImageData, imgDataSize) != imgDataSize)
        {
          FreeVec(pImageData);
          cleanup();
          throw "SaveAmosAbk: Wrong size of bytes written.";
        }

        FreeVec(pImageData);
      }
    }
  }

  // Create and write the OCS color table
  m_pOCSColorTable = colors32ToOCSColorTable(pColors32);
  if(m_pOCSColorTable == NULL)
  {
    throw "SaveAmosAbk: Failed to create the color table.";
  }

  for(ULONG i = 0; i < 32; i++)
  {
    writeWord(m_pOCSColorTable[i]);
  }
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


WORD* SaveAmosAbk::sheetBitmapToFrameImage(struct BitMap* pSrcBitmap, 
                                           ULONG xStart,
                                           ULONG sheetNumFrames,
                                           ULONG& bufSizeBytes)
{
  if(pSrcBitmap == NULL)
  {
    throw "OpenImageDataPicture: No source BitMap provided.";
  }

  ULONG width = GetBitMapAttr(pSrcBitmap, BMA_WIDTH) / sheetNumFrames;
  ULONG height = GetBitMapAttr(pSrcBitmap, BMA_HEIGHT);
  ULONG depth = GetBitMapAttr(pSrcBitmap, BMA_DEPTH);

  // Create a destination BitMap
  struct BitMap bitmap;
  InitBitMap(&bitmap, depth, width, height);

  // Allocate memory for the planes of destination Bitmap
  ULONG planeSize = RASSIZE(width, height);
  bufSizeBytes = planeSize * depth;

  WORD* pImageData = (WORD*)AllocVec(bufSizeBytes, MEMF_CHIP|MEMF_CLEAR);
  if(pImageData == NULL)
  {
    return NULL;
  }

  // Manually set all plane pointers to the dedicated area of 
  // destination Bitmap
  PLANEPTR ptr = (PLANEPTR)pImageData;
  for(ULONG i = 0; i < depth; i++)
  {
    bitmap.Planes[i] = ptr;
    ptr += planeSize;
  }

  // Blit source BitMap to destination BitMap
  BltBitMap(pSrcBitmap, 
            xStart,
            0,
            &bitmap,
            0,
            0,
            width,
            height,
            0xC0, 
            0xFF, 
            NULL);

  return pImageData;
}


ULONG* SaveAmosAbk::colors32ToOCSColorTable(ULONG* pColors32)
{
  ULONG i, red32, green32, blue32, red, green, blue;
  ULONG* pCol;
  ULONG colorWord, iRed, iGreen, iBlue;
  const ULONG numColorsOcsColorTable = 32;
  ULONG numSrcColors;
  ULONG* pColorTable;

  if(pColors32 == NULL)
  {
    return NULL;
  }

  /* get the number of colors of src colors 32 table */
  numSrcColors = (pColors32[0] & 0xffff0000) >> 16;
  if(numSrcColors > numColorsOcsColorTable)
  {
    /* Only convert as many colors as fit into the ocs color table */
    numSrcColors = numColorsOcsColorTable;
  }

  pColorTable = (ULONG*) AllocVec(numColorsOcsColorTable * sizeof(ULONG), 
                                  MEMF_PUBLIC|MEMF_CLEAR);
  if(pColorTable == NULL)
  {
    return NULL;
  }

  /* Use another pointer to skip Colors32 header item */
  pCol = pColors32 + 1;

  for(i = 0; i < numColorsOcsColorTable; i++)
  {
    colorWord = 0;
    if(i < numSrcColors)
    {
      iRed = (i + 1) * 3 - 3;
      iGreen = (i + 1) * 3 - 2;
      iBlue = (i + 1) * 3 - 1;

      red32 = pCol[iRed] >> 24;
      green32 = pCol[iGreen] >> 24;
      blue32 = pCol[iBlue] >> 24;

      red = red32 * 15.0f / 255.0f + 0.5f;
      green = green32 * 15.0f / 255.0f + 0.5f;
      blue = blue32 * 15.0f / 255.0f + 0.5f;

      colorWord = (red << 8) | (green << 4) | blue;
    }

    pColorTable[i] = colorWord;
  }


  return pColorTable;
}


void SaveAmosAbk::writeWord(ULONG value)
{
  UBYTE buf[2];
  buf[0] = (value & 0xff00) >> 8;
  buf[1] = value & 0xff;

  if(Write(m_FileHandle, &buf, 2) != 2)
  {
    cleanup();
    throw "SaveAmosAbk: Wrong size of bytes written.";
  }
}
