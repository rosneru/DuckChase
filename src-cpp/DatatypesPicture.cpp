#include "DatatypesPicture.h"


DatatypesPicture::DatatypesPicture(const char* p_pFilePath)
  : m_pFilePath(p_pFilePath),
    m_pObject(NULL),
    m_pBitmap(NULL),
    m_pBitmapHeader(NULL),
    m_pPalette(NULL),
    m_NumberOfColors(0)
{
}


DatatypesPicture::~DatatypesPicture()
{
  clear();
}


bool DatatypesPicture::Load(struct Screen* p_pScreen)
{
  clear();

  m_pObject = NewDTObject((void*)m_pFilePath,
                          DTA_GroupID, GID_PICTURE,
                          PDTA_Remap, TRUE,
                          PDTA_Screen, p_pScreen,
                          TAG_END);

  if (m_pObject == NULL)
  {
    return false;
  }

  DoDTMethod(m_pObject, NULL, NULL, DTM_PROCLAYOUT, NULL, TRUE);

  GetDTAttrs(m_pObject,
             PDTA_NumColors, &m_NumberOfColors,
             PDTA_CRegs, &m_pPalette,
             PDTA_BitMapHeader, &m_pBitmapHeader,
             PDTA_DestBitMap, &m_pBitmap,
             TAG_END);

  return true;
}


struct BitMap* DatatypesPicture::GetBitmap()
{
  return m_pBitmap;
}

struct BitMapHeader* DatatypesPicture::GetBitmapHeader()
{
  return m_pBitmapHeader;
}


ULONG* DatatypesPicture::GetPalette()
{
  return m_pPalette;
}


long DatatypesPicture::GetNumberOfColors()
{
  return m_NumberOfColors;
}


void DatatypesPicture::clear()
{
  if((m_pBitmap != NULL)
    || (m_pBitmapHeader != NULL)
    || (m_pPalette != NULL))
  {
    DisposeDTObject(m_pObject);
    m_pObject = NULL;
    m_pBitmap = NULL;
    m_pBitmapHeader = NULL;
    m_pPalette = NULL;
    m_NumberOfColors = 0;
  }
}
