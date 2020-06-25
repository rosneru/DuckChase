#ifndef ILBM_BITMAP_H
#define ILBM_BITMAP_H

#include <graphics/gfx.h>
#include <libraries/iffparse.h>

#include "BitmapPictureBase.h"


/**
 * Class for loading an iff ilbm file into a Bitmap by using the 
 * iffparse.library. Code of newiff from Commodore is heavily used.
 *
 * @author Uwe Rosner
 * @date 04/04/2020
 */
class IlbmBitmap : public BitmapPictureBase
{
public: 
  /**
   * Creates a BitMap image by loading the ILBM file with given name.
   * Dimensions of the image are extracted from the ilbm header an can
   * be retrieved after successful loading using the getters. 
   */
  IlbmBitmap(const char* pFileName,
             bool bLoadColors,
             bool bLoadDisplayMode);

  virtual ~IlbmBitmap();





private:
  struct IFFHandle* m_pIffHandle;
  const ULONG m_MaxSrcPlanes;

  bool loadColors(struct StoredProperty* pCmapProp);

  bool decodeIlbmBody(bool isCompressed, UBYTE masking);

  bool unpackRow(BYTE** ppSource, 
                 BYTE** ppDest, 
                 WORD srcBytes,
                 WORD dstBytes);

  /**
   * Returns the ID of the current chunk (e.g. ID_CAMG)
   */
  LONG currentChunkIs(struct IFFHandle* pIffHandle, LONG type, LONG id);


  /**
   * Computes the worst case packed size of a "row" of bytes.
   */
  inline ULONG maxPackedSize(ULONG rowSize) 
  {
    return ( (rowSize) + ( ((rowSize)+127) >> 7 ) );
  }

  inline ULONG chunkMoreBytes(struct ContextNode* pContextNode)
  {
    return pContextNode->cn_Size - pContextNode->cn_Scan;
  }

};

#endif
