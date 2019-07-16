#ifndef PICTURE_H
#define PICTURE_H

#include <proto/exec.h>
#include <proto/datatypes.h>
#include <datatypes/pictureclass.h>
#include <string.h>


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
  struct BitMap* m_pBitMapRaw;
  struct BitMap* m_pBitMapDt;
  ULONG* m_pPaletteDt;
  long m_NumberOfColors;

  void clear();
};

#endif
