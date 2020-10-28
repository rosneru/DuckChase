#include <clib/exec_protos.h>

#include <stdio.h>

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

    struct SheetItemNode* pItemNode = new SheetItemNode();
    pItemNode->ld_Node.ln_Type = NT_USER;

    pItemNode->ld_Node.ln_Name = (char*)AllocVec(32, MEMF_PUBLIC|MEMF_CLEAR);
    sprintf(pItemNode->ld_Node.ln_Name, 
            "%02d: %dx%dx%d", 
            (i+1), 
            pPic->Width(), 
            pPic->Height(), 
            pPic->Depth());
    
    pItemNode->pSheetContainer = pPic;
    AddTail(&m_SheetList, (struct Node*)pItemNode);
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
  struct SheetItemNode* pItem = ((struct SheetItemNode*)m_SheetList.lh_Head);
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

  struct SheetItemNode* pWorkNode = (struct SheetItemNode*)(m_SheetList.lh_Head);
  struct SheetItemNode* pNextNode;
  while(pNextNode = (struct SheetItemNode*)pWorkNode->ld_Node.ln_Succ)
  {
    // Store the next node
    pNextNode = (struct SheetItemNode*)(pWorkNode->ld_Node.ln_Succ);

    // Remove all work node allocations
    if(pWorkNode->ld_Node.ln_Name != NULL)
    {
      FreeVec(pWorkNode->ld_Node.ln_Name);
      pWorkNode->ld_Node.ln_Name = NULL;
    }

    delete pWorkNode;

    // Stored nextNode will be next workNode
    pWorkNode = pNextNode;
  }
}
