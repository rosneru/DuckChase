#include <clib/exec_protos.h>


#include "SaveBitMapPictureIlbm.h"
#include "AnimSheetContainer.h"


AnimSheetContainer::AnimSheetContainer(const char* pFileName)
{
  // Setup the exec list
  m_SheetList.lh_Head = (struct Node*) &m_SheetList.lh_Tail;
  m_SheetList.lh_Tail = 0;
  m_SheetList.lh_TailPred = (struct Node*) &m_SheetList.lh_Head;
  m_SheetList.lh_Type = NT_UNKNOWN;

  // Add all (currently 1) items to list
  for(int i = 0; i < 1; i++)
  {
    // The next line will throw an exception on failure
    OpenIlbmPictureBitMap* pPic = new OpenIlbmPictureBitMap(pFileName, 
                                                            true, 
                                                            false);

    struct AnimSheetItem* pItem = new AnimSheetItem();
    pItem->ld_Node.ln_Type = NT_USER;
    pItem->ld_Node.ln_Name = "Ilbm 1x sheet";
    pItem->pSheetContainer = pPic;
    AddTail(&m_SheetList, (struct Node*)pItem);
  }
}


AnimSheetContainer::~AnimSheetContainer()
{
  cleanup();
}


void AnimSheetContainer::save(const char* pFileName)
{

  OpenIlbmPictureBitMap* pPic = getCurrent();
  // Creation of the saver object already saves the picture
  // (or throws an exception)
  SaveBitMapPictureIlbm ilbmSaver(*pPic, pFileName);
}


OpenIlbmPictureBitMap* AnimSheetContainer::getCurrent()
{
  struct AnimSheetItem* pItem = ((struct AnimSheetItem*)m_SheetList.lh_Head);
  return (OpenIlbmPictureBitMap*)pItem->pSheetContainer;
}


struct List* AnimSheetContainer::getSheetList()
{
  return &m_SheetList;
}


void AnimSheetContainer::cleanup()
{
  OpenIlbmPictureBitMap* pPic = getCurrent();
  if(pPic != NULL)
  {
    delete pPic;
  }

  // TODO Delete exec list
}
