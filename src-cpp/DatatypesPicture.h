#ifndef DATATYPES_PICTUREABC
#define DATATYPES_PICTUREABC

#include <proto/exec.h>
#include <proto/datatypes.h>
#include <datatypes/pictureclass.h>
#include <string.h>


/**
 * Represents a picture object which can be loaded using datatypes.
 * After sucessfully loading the bitmap and palette can be retrieved
 * using getter methods.
 *
 * @author Uwe Rosner
 * @date 06/07/2019
 */
class DatatypesPicture
{
public:
  DatatypesPicture(const char* p_pFilePath);
  ~DatatypesPicture();

  bool Load(struct Screen* p_pScreen);

  struct BitMap* GetBitmap();
  struct BitMapHeader* GetBitmapHeader();
  ULONG* GetPalette();
  long GetNumberOfColors();

private:
  const char* m_pFilePath;
  Object *m_pObject;
  struct BitMapHeader* m_pBitmapHeader;
  struct BitMap* m_pBitmap;
  ULONG* m_pPalette;
  long m_NumberOfColors;

  void clear();
};

#endif
