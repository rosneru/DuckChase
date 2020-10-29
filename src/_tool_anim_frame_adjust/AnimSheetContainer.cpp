#include <clib/exec_protos.h>

#include <stdio.h>

#include "OpenAmosAbk.h"
#include "SaveBitMapPictureIlbm.h"
#include "AnimSheetContainer.h"


AnimSheetContainer::AnimSheetContainer(const char* pFileName)
  : m_SheetDataType(SDT_None)
{
  // Initialize the exec list
  m_SheetList.lh_Head = (struct Node*) &m_SheetList.lh_Tail;
  m_SheetList.lh_Tail = 0;
  m_SheetList.lh_TailPred = (struct Node*) &m_SheetList.lh_Head;
  m_SheetList.lh_Type = NT_UNKNOWN;


  try
  {
    // At first try to open the given file as an ilbm picture
    OpenIlbmPictureBitMap* pPic = new OpenIlbmPictureBitMap(pFileName, 
                                                            true, 
                                                            false);

    // Open as ILBM worked
    m_SheetDataType = SDT_IlbmPicture;

    // now add the single node
    if(addItemNode(pPic, 0) == false)
    {
      throw "Failed to create node for ILBM sheet.\n";
    }
  }
  catch(const char* pErr)
  {
    // Open as ILBM didn't work
  }
  
  // Now try to open it as AMOS .abk file
  try
  {
    OpenAmosAbk* pPic = new OpenAmosAbk(pFileName);

    m_SheetDataType = SDT_AmosBank;

    // while(pSheet = pPic.parseNextSheet())
    //   addItemNode()
    //
    // m_pColors32 = deepCopy(pPic.parseColors32)
  }
  catch(const char* pErr)
  {
    
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


bool AnimSheetContainer::addItemNode(BitMapPictureBase* pPic, 
                                     size_t initialIndex)
{
  struct SheetItemNode* pItemNode;
  pItemNode = (struct SheetItemNode*) AllocVec(sizeof(SheetItemNode), 
                                               MEMF_PUBLIC|MEMF_CLEAR);
  if(pItemNode == NULL)
  {
    return false;
  }

  pItemNode->ld_Node.ln_Type = NT_USER;

  pItemNode->ld_Node.ln_Name = (char*)AllocVec(32, MEMF_PUBLIC|MEMF_CLEAR);
  if(pItemNode->ld_Node.ln_Name == NULL)
  {
    FreeVec(pItemNode);
    return false;
  }

  sprintf(pItemNode->ld_Node.ln_Name, 
          "%02d: %dx%dx%d", 
          (initialIndex + 1), 
          pPic->Width(), 
          pPic->Height(), 
          pPic->Depth());
  
  pItemNode->pSheetContainer = pPic;
  AddTail(&m_SheetList, (struct Node*)pItemNode);
}


void AnimSheetContainer::cleanup()
{
  struct SheetItemNode* pWorkNode = (struct SheetItemNode*)(m_SheetList.lh_Head);
  struct SheetItemNode* pNextNode;
  while((pNextNode = (struct SheetItemNode*)pWorkNode->ld_Node.ln_Succ) != NULL)
  {
    // Store the next node
    pNextNode = (struct SheetItemNode*)(pWorkNode->ld_Node.ln_Succ);

    if(pWorkNode->pSheetContainer != NULL)
    {
      if(m_SheetDataType == SDT_IlbmPicture)
      {
        OpenIlbmPictureBitMap* pPic;
        pPic = (OpenIlbmPictureBitMap*) pWorkNode->pSheetContainer;
        delete pPic;
        pWorkNode->pSheetContainer = NULL;
      }
      else if(m_SheetDataType == SDT_AmosBank)
      {
        // TODO delete amos data
      }
    }

    // Remove all work node allocations
    if(pWorkNode->ld_Node.ln_Name != NULL)
    {
      FreeVec(pWorkNode->ld_Node.ln_Name);
      pWorkNode->ld_Node.ln_Name = NULL;
    }

    FreeVec(pWorkNode);

    // Stored nextNode will be next workNode
    pWorkNode = pNextNode;
  }
}
