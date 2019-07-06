#include "DatatypesPicture.h"


DatatypesPicture::DatatypesPicture()
  : m_pObject(NULL),
    m_pBitmap(NULL),
    m_pPalette(NULL),
    m_NumberOfColors(0)
{
}


DatatypesPicture::~DatatypesPicture()
{
  clear();
}


bool DatatypesPicture::Load(char *p_pFileName)
{
  clear();

  m_pObject = NewDTObject(p_pFileName,
                          DTA_GroupID, GID_PICTURE,
                          PDTA_Remap, FALSE,
                          TAG_END);

  if (m_pObject == NULL)
  {
    return false;
  }

  DoDTMethod(m_pObject, NULL, NULL, DTM_PROCLAYOUT, NULL, TRUE);

  GetDTAttrs(m_pObject,
             PDTA_NumColors, &m_NumberOfColors,
             PDTA_CRegs, &m_pPalette,
             PDTA_DestBitMap, &m_pBitmap,
             TAG_END);

  return true;
}


struct BitMap* DatatypesPicture::GetBitmap()
{
  return m_pBitmap;
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
  if((m_pBitmap != NULL) || (m_pPalette != NULL))
  {
    DisposeDTObject(m_pObject);
    m_pObject = NULL;
    m_pBitmap = NULL;
    m_pPalette = NULL;
    m_NumberOfColors = 0;
  }
}
