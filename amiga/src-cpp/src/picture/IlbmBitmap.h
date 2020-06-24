#ifndef ILBM_BITMAP_H
#define ILBM_BITMAP_H

#include <datatypes/pictureclass.h> // No usage of datatypes, this is just 
                                    // for 'struct BitMapHeader' etc.
#include <graphics/gfx.h>
#include <libraries/iffparse.h>

#include "BitmapPictureBase.h"


//  IFF types we may encounter
#define	ID_ILBM		MAKE_ID('I','L','B','M')

// ILBM Chunk ID's we may encounter
#define	ID_BMHD		MAKE_ID('B','M','H','D')
#define	ID_BODY		MAKE_ID('B','O','D','Y')
#define	ID_CMAP		MAKE_ID('C','M','A','P')
#define	ID_CRNG		MAKE_ID('C','R','N','G')
#define	ID_CCRT		MAKE_ID('C','C','R','T')
#define	ID_GRAB		MAKE_ID('G','R','A','B')
#define	ID_SPRT		MAKE_ID('S','P','R','T')
#define	ID_DEST		MAKE_ID('D','E','S','T')
#define	ID_CAMG		MAKE_ID('C','A','M','G')

/**
 * Class for loading a iff ilbm image file into a Bitmap using the 
 * iffparse.library. Code of newiff from Commodore is heavily used.
 *
 * @author Uwe Rosner
 * @date 04/04/2020
 */
class IlbmBitmap : public BitmapPictureBase
{
public: 
  /**
   * Creates a BitMap file by loading the ILBM image of given filename.
   * Dimensions of the image are extracted from the ilbm header an can
   * be retrieved after successful loading using the getters. 
   *
   * @returns On success: true, on failure: false
   */
  IlbmBitmap(const char* pFileName,
             bool bLoadColors,
             bool bLoadDisplayMode);

  virtual ~IlbmBitmap();





private:
  struct IFFHandle* m_pIffHandle;

  bool loadColors(struct StoredProperty* pCmapProp);

  bool decodeIlbmBody(bool isCompressed, UBYTE masking = mskNone);

  bool unpackRow(BYTE** ppSource, 
                 BYTE** ppDest, 
                 WORD srcBytes,
                 WORD dstBytes);

  /**
   * Returns the ID of the current chunk (e.g. ID_CAMG)
   */
  LONG currentChunkIs(struct IFFHandle* pIffHandle, LONG type, LONG id);
};

#endif
