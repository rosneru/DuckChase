#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/iffparse_protos.h>
#include <dos/dos.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>

#include "IlbmBitMap.h"

extern struct GfxBase* GfxBase;


IlbmBitMap::IlbmBitMap(const char* pFileName,
                       bool bLoadColors,
                       bool bLoadDisplayMode)
  : BitMapPictureBase(),
    m_pIffHandle()
{
  if (pFileName == NULL)
  {
    throw "IlbmBitMap: No file name provided.";
  }

  m_pIffHandle = AllocIFF();
  if(m_pIffHandle == NULL)
  {
    throw "IlbmBitMap: Failed to AllocIFF.";
  }

  m_pIffHandle->iff_Stream = Open(pFileName, MODE_OLDFILE);
  if (m_pIffHandle->iff_Stream == 0)
  {
    throw "IlbmBitMap: Failed to open file.";
  }

  InitIFFasDOS(m_pIffHandle);

  LONG iffErr;
  if((iffErr = OpenIFF(m_pIffHandle, IFFF_READ)) != 0)
  {
    throw "IlbmBitMap: OpenIFF returned error.";
  }

  // Define which chunks to load
  PropChunk(m_pIffHandle, ID_ILBM, ID_BMHD);

  if(bLoadColors)
  {
    PropChunk(m_pIffHandle, ID_ILBM, ID_CMAP);
  }

  if(bLoadDisplayMode)
  {
    PropChunk(m_pIffHandle, ID_ILBM, ID_CAMG);
  }

  StopChunk(m_pIffHandle, ID_ILBM, ID_BODY);

  // Parse the iff file
  iffErr = ParseIFF(m_pIffHandle, IFFPARSE_SCAN);
  if(iffErr != 0)
  {
    throw "IlbmBitMap: Error in ParseIFF.";
  }

  // Load the BitMap header
  StoredProperty* pStoredProp = FindProp(m_pIffHandle, ID_ILBM, ID_BMHD);
  if(pStoredProp == NULL)
  {
    throw "IlbmBitMap: No BitMap header found in ilbm picture.";
  }

  BitMapHeader* pBitMapHeader = (BitMapHeader*)pStoredProp->sp_Data;
  if(pBitMapHeader == NULL)
  {
    throw "IlbmBitMap: Bitmap header of ilbm picture is empty.";
  }

  // Allocate the BitMap
  m_pBitMap = AllocBitMap(pBitMapHeader->bmh_Width,
                          pBitMapHeader->bmh_Height,
                          pBitMapHeader->bmh_Depth,
                          BMF_CLEAR, //|BMF_INTERLEAVED,
                          NULL);

  if (m_pBitMap == NULL)
  {
      throw "IlbmBitMap: Failed to AllocBitMap.";
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

  // if(loadCAMG)
  // {
  //   // TODO Get CAMG
  // }
  if(bLoadColors)
  {
    // Load the color map
    pStoredProp = FindProp(m_pIffHandle, ID_ILBM, ID_CMAP);
    if(pStoredProp != NULL)
    {
      if(loadColors(pStoredProp) == false)
      {
        throw "IlbmBitMap: Error while loading the colors from ilbm cmap.";
      }
    }
  }

  if(decodeIlbmBody(isCompressed,
                    pBitMapHeader->bmh_Masking) == false)
  {
    throw "IlbmBitMap: Error while decoding the ilbm body.";
  }

  Close(m_pIffHandle->iff_Stream);
  m_pIffHandle->iff_Stream = 0;

  CloseIFF(m_pIffHandle);
  m_pIffHandle = NULL;
}

IlbmBitMap::~IlbmBitMap()
{
  if(m_pIffHandle != NULL)
  {
    if(m_pIffHandle->iff_Stream != 0)
    {
      Close(m_pIffHandle->iff_Stream);
    }

    CloseIFF(m_pIffHandle);
  }
}



bool IlbmBitMap::loadColors(struct StoredProperty* pCmapProp)
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


/**
 * Macro from newiff/packer.h
 * This macro computes the worst case packed size of a "row" of bytes.
 */
#define MaxPackedSize(rowSize)  ( (rowSize) + ( ((rowSize)+127) >> 7 ) )

/**
 * Macro from newiff/iff.h
 */
#define ChunkMoreBytes(cn)	(cn->cn_Size - cn->cn_Scan)

/*
 * Macro from newiff/ilbm.h
 *
 * Convert image width to even number of BytesPerRow for ILBM save.
 * Do NOT use this macro to determine the actual number of bytes per row
 * in an Amiga BitMap.  Use BitMap->BytesPerRow for scan-line modulo.
 * Use your screen or viewport width to determine width. Or under
 * V39, use GetBitMapAttr().
 */
#define RowBytes(w)	((((w) + 15) >> 4) << 1)
#define RowBits(w)	((((w) + 15) >> 4) << 4

/**
 * See newiff/ilbmr.c
 */
#define MaxSrcPlanes (25)


bool IlbmBitMap::decodeIlbmBody(bool isCompressed,
                                UBYTE masking,
                                BYTE* mask)
{
  if(m_pBitMap == NULL)
  {
    return false;
  }

  if (!(currentChunkIs(m_pIffHandle, ID_ILBM, ID_BODY)))
  {
    // No body chunk. Maybe it's a palette.
    return false;
  }

  ULONG bufSize = MaxPackedSize(RowBytes(Width())) << 4;
  BYTE* pAllocatedBuf = (BYTE*)AllocVec(bufSize, MEMF_ANY);
  BYTE* pBuffer = pAllocatedBuf;
  if (pBuffer == NULL)
  {
    return false;
  }

  struct ContextNode* cn = CurrentChunk(m_pIffHandle);

  WORD srcRowBytes = RowBytes(Width());
  WORD destRowBytes = m_pBitMap->BytesPerRow; // used as a modulo only
  LONG bufRowBytes = MaxPackedSize(srcRowBytes);

  // Initialize array "planes" with bitmap ptrs; NULL in empty slots.
  BYTE* planes[MaxSrcPlanes]; // array of ptrs to planes & mask
  ULONG iPlane = 0;
  for (iPlane = 0; iPlane < m_pBitMap->Depth; iPlane++)
  {
    planes[iPlane] = (BYTE*)m_pBitMap->Planes[iPlane];
  }

  for (; iPlane < MaxSrcPlanes; iPlane++)
  {
    planes[iPlane] = NULL;
  }

  // Copy any mask plane ptr into corresponding "planes" slot.
  UBYTE srcPlaneCnt = Depth();
  if (masking == mskHasMask)
  {
    if (mask != NULL)
    {
      // If there are more srcPlanes than dstPlanes, there will be NULL
      // plane-pointers before this.
      planes[srcPlaneCnt] = mask;
    }
    else
    {
      planes[srcPlaneCnt] = NULL; /* In case more dstPlanes than src.*/
    }

    // Include mask plane in count.
    srcPlaneCnt++;
  }

  // Setup a sink for dummy destination of rows from unwanted planes.
  BYTE* nullDest = pBuffer;
  pBuffer += srcRowBytes;
  bufSize -= srcRowBytes;

  // Read the BODY contents into client's bitmap. De-interleave planes
  // and decompress rows. MODIFIES: Last iteration modifies bufsize.
  BYTE* pBuf = pBuffer + bufSize; // Buffer is currently empty.
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
      WORD nEmpty = pBuf - pBuffer;     //size of empty part of buffer.
      WORD nFilled = bufSize - nEmpty;  //this part has data.
      if (nFilled < bufRowBytes)
      {
        // Need to read more.

        // Move the existing data to the front of the buffer. Now covers
        // range buffer[0]..buffer[nFilled-1].
        CopyMem(pBuf, pBuffer, nFilled);  // Could be moving 0 bytes.

        if (nEmpty > ChunkMoreBytes(cn))
        {
          // There aren't enough bytes left to fill the buffer.
          nEmpty = ChunkMoreBytes(cn);
          bufSize = nFilled + nEmpty; // heh-heh
        }

        // Append new data to the existing data
        LONG iffResult = ReadChunkBytes(m_pIffHandle,
                                        &pBuffer[nFilled],
                                        nEmpty);
        if (iffResult < nEmpty)
        {
          FreeVec(pAllocatedBuf);
          return false;
        }

        pBuf = pBuffer;
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
          FreeVec(pAllocatedBuf);
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
          FreeVec(pAllocatedBuf);
          return false;
        }

        CopyMem(pBuf, *pDest, srcRowBytes);
        pBuf += srcRowBytes;
        *pDest += destRowBytes;
      }
    }
  }

  FreeVec(pAllocatedBuf);
  return true;
}



bool IlbmBitMap::unpackRow(BYTE** ppSource,
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


LONG IlbmBitMap::currentChunkIs(struct IFFHandle* pIffHandle,
                                         LONG type,
                                         LONG id)
{
  struct ContextNode* pContextNode;
  LONG result = 0;

  if ((pContextNode = CurrentChunk(pIffHandle)) != NULL)
  {
    if ((pContextNode->cn_Type == type) && (pContextNode->cn_ID == id))
    {
      result = 1;
    }
  }

  return result;
}
