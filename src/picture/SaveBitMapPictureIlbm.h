#ifndef SAVE_BITMAP_ILBM_PICTURE_H
#define SAVE_BITMAP_ILBM_PICTURE_H

#include <datatypes/pictureclass.h> // Not using  datatypes, just for
                                    // 'BitMapHeader', 'ID_xyz', etc.

#include <graphics/gfx.h>
#include "IffParse.h"

/**
 * Class for saving a BitMap to an iff ilbm file using the
 * iffparse.library. Code of newiff39 from Commodore is used.
 *
 * @author Uwe Rosner
 * @date 17/09/2020
 */
class SaveBitMapPictureIlbm
{
public:
  SaveBitMapPictureIlbm(const char* pFileName,
                        struct BitMap* pBitMap,
                        ULONG* pColors32,
                        ULONG modeId);

  virtual ~SaveBitMapPictureIlbm();

private:
  struct BitMap* m_pBitMap;
  ULONG* m_pColors32;
  ULONG m_ModeId;

  ULONG m_SrcWidth;
  ULONG m_SrcHeight;
  ULONG m_SrcDepth;

  const ULONG m_BODY_BUF_SIZE;
  const ULONG m_MAX_SAVE_DEPTH;
  struct BitMapHeader m_Bmhd;
  UBYTE* bodybuf;

  void cleanup();

  /**
   * Initialize the BitMapHeader
   */
  void initBitMapHeader();

  /**
   * Write the pictures Colors32 into the ilbm files CMAP.
   */
  long PutCmap(IffParse& iff);

  /**
   * Write the picture BitMap data into the ilbm files BODY.
   */
  long PutBody(IffParse& iff);

  /**
   * packs one row, updating the source and destination pointers.
   * 
   * NOTE: Given POINTERS TO POINTERS, 
   * 
   * @returns Count of packed bytes.
   */
  LONG packrow(UBYTE** ppSource, UBYTE** ppDest, LONG rowSize);
};

#endif
