#ifndef ILBM_BITMAP_H
#define ILBM_BITMAP_H

#include <graphics/gfx.h>

#include "BitmapPictureBase.h"
#include "IffParse.h"

/**
 * Class for loading an iff ilbm file into a Bitmap by using the 
 * iffparse.library. Code of newiff39 from Commodore is used.
 *
 * @author Uwe Rosner
 * @date 04/04/2020
 */
class OpenIlbmPictureBitMap : public BitmapPictureBase
{
public: 
  /**
   * Creates a BitMap image by loading the ILBM file with given name.
   * Dimensions of the image are extracted from the ilbm header an can
   * be retrieved after successful loading using the getters. 
   */
  OpenIlbmPictureBitMap(const char* pFileName,
             bool bLoadColors,
             bool bLoadDisplayMode);

  virtual ~OpenIlbmPictureBitMap();





private:
  const ULONG m_MaxSrcPlanes;

  bool loadColors(struct StoredProperty* pCmapProp);
  bool loadDisplayMode(struct StoredProperty* pCamgProp, 
                       struct BitMapHeader* pBitMapHeader);

  bool decodeIlbmBody(IffParse& iffParse, 
                      bool isCompressed, 
                      UBYTE masking);

  bool unpackRow(BYTE** ppSource, 
                 BYTE** ppDest, 
                 WORD srcBytes,
                 WORD dstBytes);

};

#endif
