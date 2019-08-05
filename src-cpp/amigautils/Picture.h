#ifndef PICTURE_H
#define PICTURE_H

#include <datatypes/pictureclass.h>


/**
 * Represents a picture which after successful loading at least
 * contains a bitmap which can be accessed from outside.
 *
 *
 * Loading methods exists for RAW files or for loading any datatypes
 * supported image type on OS3.0+ systems.
 *
 * @author Uwe Rosner
 * @date 06/07/2019
 */
class Picture
{
public:
  Picture();
  ~Picture();

  bool LoadFromDatatype(const char* p_pPath,
                        struct Screen* p_pScreen);

  bool LoadFromRawFile(const char* p_pFilePath,
                       int p_ImageWidth,
                       int p_ImageHeight,
                       short p_ImageDepth);

  struct BitMap* GetBitMap();
  struct BitMapHeader* GetBitMapHeader();
  ULONG* GetPalette();
  long GetNumberOfColors();

private:
  Object *m_pObject;
  struct BitMapHeader* m_pBitMapDtHeader;
  ULONG* m_pPaletteDt;
  struct BitMap* m_pBitMapDt;
  struct BitMap* m_pBitMapRaw;
  UBYTE* m_pPlaneMemoryRaw;
  long m_NumberOfColors;

  void clear();
};

#endif
