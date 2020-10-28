#include "SaveBitMapPictureIlbm.h"
#include "AnimSheetContainer.h"


OpenAnimSheets::OpenAnimSheets(const char* pFileName)
  : m_pPicture(NULL)
{
  // Creating the OpenIlbmPictureBitMap will throw an exception on failure
  m_pPicture = new OpenIlbmPictureBitMap(pFileName, true, false);
}


OpenAnimSheets::~OpenAnimSheets()
{
  cleanup();
}


void OpenAnimSheets::save(const char* pFileName)
{
  // Creation of the saver object already saves the picture
  // (or throws an exception)
  SaveBitMapPictureIlbm ilbmSaver(*m_pPicture, pFileName);
}


OpenIlbmPictureBitMap* OpenAnimSheets::getCurrent()
{
  return m_pPicture;
}


void OpenAnimSheets::cleanup()
{
  if(m_pPicture != NULL)
  {
    delete m_pPicture;
    m_pPicture = NULL;
  }
}
