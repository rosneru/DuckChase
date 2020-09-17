#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/iffparse_protos.h>
#include <datatypes/pictureclass.h> // Not using  datatypes, just for
                                    // 'BitMapHeader', 'ID_xyz', etc.
#include <exec/memory.h>

#include "IlbmBitmap.h"


IlbmBitmap::IlbmBitmap(const char* pFileName,
                       bool bLoadColors,
                       bool bLoadDisplayMode)
  : BitmapPictureBase(),
    m_MaxSrcPlanes(25)
{
  IffParse iffParse(pFileName);

  LONG iffErr;
  if((iffErr = OpenIFF(iffParse.Handle(), IFFF_READ)) != 0)
  {
    throw "IlbmBitmap: OpenIFF returned error.";
  }

  // Define which chunks to load
  PropChunk(iffParse.Handle(), ID_ILBM, ID_BMHD);

  if(bLoadColors)
  {
    PropChunk(iffParse.Handle(), ID_ILBM, ID_CMAP);
  }

  if(bLoadDisplayMode)
  {
    PropChunk(iffParse.Handle(), ID_ILBM, ID_CAMG);
  }

  StopChunk(iffParse.Handle(), ID_ILBM, ID_BODY);

  // Parse the iff file
  iffErr = ParseIFF(iffParse.Handle(), IFFPARSE_SCAN);
  if(iffErr != 0)
  {
    CloseIFF(iffParse.Handle());
    throw "IlbmBitmap: Error in ParseIFF.";
  }

  // Load the BitMap header
  StoredProperty* pStoredProp = FindProp(iffParse.Handle(), ID_ILBM, ID_BMHD);
  if(pStoredProp == NULL)
  {
    CloseIFF(iffParse.Handle());
    throw "IlbmBitmap: No BitMap header found in file.";
  }

  BitMapHeader* pBitMapHeader = (BitMapHeader*)pStoredProp->sp_Data;
  if(pBitMapHeader == NULL)
  {
    CloseIFF(iffParse.Handle());
    throw "IlbmBitmap: Bitmap header of ilbm file is empty.";
  }

  // Allocate the BitMap
  m_pBitMap = AllocBitMap(pBitMapHeader->bmh_Width,
                          pBitMapHeader->bmh_Height,
                          pBitMapHeader->bmh_Depth,
                          BMF_CLEAR, //|BMF_INTERLEAVED,
                          NULL);

  if (m_pBitMap == NULL)
  {
    CloseIFF(iffParse.Handle());
    throw "IlbmBitmap: Failed to AllocBitMap.";
  }

  bool isCompressed = false;
  if(pBitMapHeader->bmh_Compression == cmpNone)
  {
    isCompressed = false;
  }
  else if(pBitMapHeader->bmh_Compression == cmpByteRun1)
  {
    isCompressed = true;
  }

  if(bLoadDisplayMode)
  {
    // Load the CAMG
    pStoredProp = FindProp(iffParse.Handle(), ID_ILBM, ID_CAMG);
    if(pStoredProp != NULL)
    {
      if(loadDisplayMode(pStoredProp, pBitMapHeader) == false)
      {
        CloseIFF(iffParse.Handle());
        throw "IlbmBitmap: Error while loading the display mode from ILBM CAMG.";
      }
    }
  }

  if(bLoadColors)
  {
    // Load the color map
    pStoredProp = FindProp(iffParse.Handle(), ID_ILBM, ID_CMAP);
    if(pStoredProp != NULL)
    {
      if(loadColors(pStoredProp) == false)
      {
        CloseIFF(iffParse.Handle());
        throw "IlbmBitmap: Error while loading the colors from ILBM CMAP.";
      }
    }
  }

  if(decodeIlbmBody(iffParse,
                    isCompressed,
                    pBitMapHeader->bmh_Masking) == false)
  {
    CloseIFF(iffParse.Handle());
    throw "IlbmBitmap: Error while decoding the ilbm body.";
  }

  CloseIFF(iffParse.Handle());
}

IlbmBitmap::~IlbmBitmap()
{

}



bool IlbmBitmap::loadColors(struct StoredProperty* pCmapProp)
{
  if(m_pColors32 != NULL)
  {
    return true;
  }

  if(pCmapProp == NULL)
  {
    return false;
  }

  // CMAP contains this many colors
  ULONG numIlbmColors = pCmapProp->sp_Size / 3;

  // Allocate memory for all colors in CMAP
  ULONG  numAllocatedColors = numIlbmColors;
  if(numAllocatedColors < 32)
  {
    // But  at least 32 colors
    numAllocatedColors = 32;
  }

  // Create array for the colors
  ULONG arraySize = ((2 + 3 * numAllocatedColors) * sizeof(ULONG));
  m_pColors32 = (ULONG*)AllocVec(arraySize, MEMF_PUBLIC|MEMF_CLEAR);
  m_pColors32[0] = numAllocatedColors << 16;


  BYTE* pRgb = (BYTE*) pCmapProp->sp_Data;

  ULONG* pColorTable = m_pColors32;
  pColorTable++; // Skipping first item which contains the number of colors

  bool allShifted = true;
  ULONG numItemsToCheck = numIlbmColors * 3;
  for (ULONG k = 0; k < numItemsToCheck; k++)
  {
    m_pColors32[k + 1] = *pRgb++;

    if(m_pColors32[k + 1] & 0x0F)
    {
      allShifted = false;
    }
  }

  if(allShifted)
  {
    // All nibbles appear shifted (4 bit), so duplicate them
    for(ULONG k = 0; k < numItemsToCheck; k++)
    {
      m_pColors32[k + 1] |= m_pColors32[k + 1] >> 4;
    }

  }

  // Now scale to 32 bits
  for(ULONG k = 0; k < numItemsToCheck; k++)
  {
    ULONG gun = m_pColors32[k + 1] & 0xff;
    m_pColors32[k + 1] = gun << 24;
  }

  return true;
}

bool IlbmBitmap::loadDisplayMode(struct StoredProperty* pCamgProp, 
                                 struct BitMapHeader* pBitMapHeader)
{
  m_ModeId = (*(ULONG*)pCamgProp->sp_Data);

  // Filter 1: Knock bad bits out of old-style 16-bit viewmode CAMGs
  if (((m_ModeId & MONITOR_ID_MASK) == 0) ||
      ((m_ModeId & EXTENDED_MODE) && (!(m_ModeId & 0xFFFF0000))))
  {
    m_ModeId &= (~(EXTENDED_MODE | SPRITES | GENLOCK_AUDIO | GENLOCK_VIDEO | VP_HIDE));
  }

  // Filter 2: Check for bogus CAMG like DPaintII brushes with junk in
  // upper word and extended bit not set in lower word.
  if ((m_ModeId & 0xFFFF0000) && (!(m_ModeId & 0x00001000)))
  {
    // No (or bad) CAMG present; calculate the mode dependent on the 
    // pixels
    m_ModeId = 0L;
    if (pBitMapHeader->bmh_Width >= 640)
    {
      m_ModeId = HIRES;
    }

    if (pBitMapHeader->bmh_Height >= 400)
    {
      m_ModeId |= LACE;
    }

    // Detect EHB, HAM, HAM8: Extra-Halfbrite files are identified by
    // bit 7 of the CAMG chunk being set. HAM6/HAM8 files have bit 11 of
    // the CAMG chunk set and a depth of 6 / 8.
    ULONG readCAMG = (*(ULONG*)pCamgProp->sp_Data);

    if ((pBitMapHeader->bmh_Depth == 6) && ((readCAMG & 0x40) != 0))
    {
      // Bit 7 of the CAMG chunk set
      m_ModeId |= EXTRA_HALFBRITE;
    }
    else if ((pBitMapHeader->bmh_Depth == 6) && ((readCAMG & 0x400) != 0))
    {
        // Bit 11 of the CAMG chunk set
        m_ModeId |= HAM;  
    }
    else if ((pBitMapHeader->bmh_Depth == 8) && ((readCAMG & 0x400) != 0))
    {
        // Bit 11 of the CAMG chunk set
        m_ModeId |= HAM;  
    }
  }

  return true;
}

bool IlbmBitmap::decodeIlbmBody(IffParse& iffParse, 
                                bool isCompressed, 
                                UBYTE masking)
{
  if(m_pBitMap == NULL)
  {
    return false;
  }

  if (!(iffParse.currentChunkIs(ID_ILBM, ID_BODY)))
  {
    // No body chunk. Maybe it's a palette.
    return false;
  }

  ULONG srcRowBytes = ((((Width()) + 15) >> 4) << 1);
  LONG bufRowBytes = iffParse.maxPackedSize(srcRowBytes);
  ULONG bufSize = bufRowBytes << 4;
  BYTE* pBufStart = (BYTE*)AllocVec(bufSize, MEMF_ANY);
  BYTE* pCurrBufPos = pBufStart;
  if (pCurrBufPos == NULL)
  {
    return false;
  }

  struct ContextNode* pContextNode = CurrentChunk(iffParse.Handle());

  WORD destRowBytes = m_pBitMap->BytesPerRow; // used as a modulo only

  // Initialize array "planes" with bitmap ptrs; NULL in empty slots.
  BYTE* planes[m_MaxSrcPlanes]; // array of ptrs to planes & mask
  ULONG iPlane = 0;
  for (iPlane = 0; iPlane < m_pBitMap->Depth; iPlane++)
  {
    planes[iPlane] = (BYTE*)m_pBitMap->Planes[iPlane];
  }

  for (; iPlane < m_MaxSrcPlanes; iPlane++)
  {
    planes[iPlane] = NULL;
  }

  // Copy any mask plane ptr into corresponding "planes" slot.
  UBYTE srcPlaneCnt = Depth();
  if (masking == mskHasMask)
  {
    m_pBitMapMask = AllocBitMap(Width(), Height(), 1, BMF_CLEAR, NULL);
    if (m_pBitMapMask != NULL)
    {
      // If there are more srcPlanes than dstPlanes, there will be NULL
      // plane-pointers before this.
      planes[srcPlaneCnt] = (BYTE*)m_pBitMapMask->Planes[0];
    }
    else
    {
      planes[srcPlaneCnt] = NULL; /* In case more dstPlanes than src.*/
    }

    // Include mask plane in count.
    srcPlaneCnt++;
  }

  // Setup a sink for dummy destination of rows from unwanted planes.
  BYTE* nullDest = pCurrBufPos;
  pCurrBufPos += srcRowBytes;
  bufSize -= srcRowBytes;

  // Read the BODY contents into client's bitmap. De-interleave planes
  // and decompress rows. MODIFIES: Last iteration modifies bufsize.
  BYTE* pBuf = pCurrBufPos + bufSize; // Buffer is currently empty.
  for (ULONG iRow = Height(); iRow > 0; iRow--)
  {
    for (ULONG iPlane = 0; iPlane < srcPlaneCnt; iPlane++)
    {
      BYTE** pDest = &planes[iPlane];

      // Establish a sink for any unwanted plane.
      if (*pDest == NULL)
      {
        BYTE* nullBuf = nullDest;
        pDest = &nullBuf;
      }

      // Read in at least enough bytes to uncompress next row.
      UWORD nEmpty = pBuf - pCurrBufPos;    //size of empty part of buffer.
      UWORD nFilled = bufSize - nEmpty; //this part has data.
      if (nFilled < bufRowBytes)
      {
        // Need to read more.

        // Move the existing data to the front of the buffer. Now covers
        // range buffer[0]..buffer[nFilled-1].
        CopyMem(pBuf, pCurrBufPos, nFilled);  // Could be moving 0 bytes.

        if (nEmpty > iffParse.chunkMoreBytes(pContextNode))
        {
          // There aren't enough bytes left to fill the buffer.
          nEmpty = iffParse.chunkMoreBytes(pContextNode);
          bufSize = nFilled + nEmpty; // heh-heh
        }

        // Append new data to the existing data
        LONG iffResult = ReadChunkBytes(iffParse.Handle(),
                                        &pCurrBufPos[nFilled],
                                        nEmpty);
        if (iffResult < nEmpty)
        {
          FreeVec(pBufStart);
          return false;
        }

        pBuf = pCurrBufPos;
        nFilled = bufSize;
        nEmpty = 0;
      }

      // Copy uncompressed row to destination plane.
      if (isCompressed)
      {
        // Decompress row to destination plane.
        if (unpackRow(&pBuf, pDest, nFilled, srcRowBytes) == false)
        {
          // Corrupted data in ilbm body of compressed ILBM file.
          FreeVec(pBufStart);
          return false;
        }
        else
        {
          *pDest += (destRowBytes - srcRowBytes);
        }
      }
      else
      {
        if (nFilled < srcRowBytes)
        {
          // Corrupted data in ilbm body of non-compressed ILBM file.
          FreeVec(pBufStart);
          return false;
        }

        CopyMem(pBuf, *pDest, srcRowBytes);
        pBuf += srcRowBytes;
        *pDest += destRowBytes;
      }
    }
  }

  FreeVec(pBufStart);
  return true;
}



bool IlbmBitmap::unpackRow(BYTE** ppSource,
                              BYTE** ppDest,
                              WORD srcBytes,
                              WORD dstBytes)
{
  BYTE* pSource = *ppSource;
  BYTE* pDest = *ppDest;

  while (dstBytes > 0)
  {
    if ((srcBytes -= 1) < 0)
    {
      return false;
    }

    WORD n = *pSource++;
    if (n >= 0)
    {
      n += 1;
      if((srcBytes -= n) < 0 || (dstBytes -= n) < 0)
      {
        return false;
      }

      do
      {
        *pDest++ = *pSource++;
      }
      while (--n > 0);
    }
    else if (n != -128)
    {
      n = -n + 1;
      if ((srcBytes -= 1) < 0 || (dstBytes -= n) < 0)
      {
        return false;
      }

      BYTE c = *pSource++;
      do
      {
        *pDest++ = c;
      }
      while (--n > 0);
    }
  }

  *ppSource = pSource;
  *ppDest = pDest;
  return true;
}
