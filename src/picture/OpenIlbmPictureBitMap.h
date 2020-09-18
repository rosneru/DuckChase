#ifndef OPEN_ILBM_PICTURE_BITMAP_H
#define OPEN_ILBM_PICTURE_BITMAP_H

#include <graphics/gfx.h>

#include "BitMapPictureBase.h"
#include "IffParse.h"

/**
 * Class for loading an iff ilbm file into a Bitmap by using the 
 * iffparse.library. Code of newiff39 from Commodore is used.
 *
 * @author Uwe Rosner
 * @date 04/04/2020
 */
class OpenIlbmPictureBitMap : public BitMapPictureBase
{
public: 
  /**
   * Load given iff ilbm file and optionally also its colors and
   * screenmode id. If the ilbm picture contains a mask it is also
   * loaded.
   */
  OpenIlbmPictureBitMap(const char* pFileName,
                        bool bLoadColors32,
                        bool bLoadModeId);

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
