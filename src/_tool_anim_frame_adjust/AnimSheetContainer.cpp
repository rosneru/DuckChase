#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include <stdio.h>

#include "OpenAmosAbk.h"
#include "SaveBitMapPictureIlbm.h"
#include "AnimSheetContainer.h"


AnimSheetContainer::AnimSheetContainer(const char* pFileName)
  : m_SheetDataType(SDT_None),
    m_NumSheets(0),
    m_pColors32(NULL)
{
  // Initialize the exec list
  m_SheetList.lh_Head = (struct Node*) &m_SheetList.lh_Tail;
  m_SheetList.lh_Tail = 0;
  m_SheetList.lh_TailPred = (struct Node*) &m_SheetList.lh_Head;
  m_SheetList.lh_Type = NT_UNKNOWN;


  try
  {
    // At first try to open the given file as an ilbm picture
    OpenIlbmPictureBitMap pic(pFileName, true, false);

    // Open as ILBM worked
    m_SheetDataType = SDT_IlbmPicture;

    // now add the single node
    if(addItemNode(pic, 0) == false)
    {
      throw "Failed to create node for ILBM sheet.\n";
    }

    m_pColors32 = deepCopyColors(pic);
  }
  catch(const char* pErr)
  {
    // Open as ILBM didn't work
  }
  
  // Now try to open it as AMOS .abk file
  try
  {
    OpenAmosAbk pic(pFileName);

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

  // TODO
  // OpenIlbmPictureBitMap* pPic = getSheetItem();
  // // Creation of the saver object already saves the picture
  // // (or throws an exception)
  // SaveBitMapPictureIlbm ilbmSaver(*pPic, pFileName);
}


struct SheetItemNode* AnimSheetContainer::getSheetItem(ULONG index)
{
  ULONG i = 0;

  // Point pNode to the header node (which contains no data)
  struct Node* pNode = m_SheetList.lh_Head;

  if(index >= m_NumSheets)
  { 
    // Wanted index is outside list bounds
    return NULL;
  }

  // As long there is a successor node
  while((pNode = pNode->ln_Succ) != NULL)
  {
    if(index == i)
    {
      // This is the wanted node
      return (struct SheetItemNode*) pNode;
    }

    i++;
    if(i >= m_NumSheets)
    { 
      // Not found
      return NULL;
    }
  }

  return NULL;
}


struct List* AnimSheetContainer::getSheetList()
{
  return &m_SheetList;
}


ULONG* AnimSheetContainer::getColors32()
{
  return m_pColors32;
}

ULONG AnimSheetContainer::getNumSheets()
{
  return m_NumSheets;
}

bool AnimSheetContainer::addItemNode(const BitMapPictureBase& pic, 
                                     ULONG initialIndex)
{
  struct SheetItemNode* pItemNode;
  pItemNode = (struct SheetItemNode*) AllocVec(sizeof(SheetItemNode), 
                                               MEMF_PUBLIC|MEMF_CLEAR);
  if(pItemNode == NULL)
  {
    return false;
  }

  pItemNode->ld_Node.ln_Type = NT_USER;

  // Alloc memory for node name
  pItemNode->ld_Node.ln_Name = (char*)AllocVec(32, MEMF_PUBLIC|MEMF_CLEAR);
  if(pItemNode->ld_Node.ln_Name == NULL)
  {
    FreeVec(pItemNode);
    return false;
  }

  // Copy current sheet dimensions into node name
  sprintf(pItemNode->ld_Node.ln_Name, 
          "%02d: %dx%dx%d", 
          (initialIndex + 1), 
          pic.Width(), 
          pic.Height(), 
          pic.Depth());
  
  pItemNode->pBitMap = AllocBitMap(pic.Width(),
                                   pic.Height(),
                                   pic.Depth(),
                                   BMF_CLEAR,
                                   pic.GetBitMap());

  if(pItemNode->pBitMap == NULL)
  {
    // Failed to allocate BitMap for this sheet
    FreeVec(pItemNode->ld_Node.ln_Name);
    FreeVec(pItemNode);
    return false;
  }

  BltBitMap(pic.GetBitMap(),
            0, 0,
            pItemNode->pBitMap,
            0, 0, 
            pic.Width(),
            pic.Width(),
            0Xc0,
            0xff,
            NULL);

  pItemNode->Width = pic.Width();
  pItemNode->Height = pic.Height();
  pItemNode->Depth = pic.Depth();

  AddTail(&m_SheetList, (struct Node*)pItemNode);
  m_NumSheets++;
  return true;
}


ULONG* AnimSheetContainer::deepCopyColors(const BitMapPictureBase& pic)
{
  ULONG numColors, colorArraySize, colorArrayByteSize;
  ULONG* pSrcBitMapColors32;
  ULONG* pColors32;

  // Copy the first n colors from source image (with n = depth)
  pSrcBitMapColors32 = pic.GetColors32();
  if(pSrcBitMapColors32 == NULL)
  {
    return NULL;
  }

  numColors = 1L << GetBitMapAttr(pic.GetBitMap(), BMA_DEPTH);

  // Size of the Colors32 table (number of ULONG values)
  colorArraySize = 3 * numColors + 2;

  // Size of the Colors32 table in bytes
  colorArrayByteSize = colorArraySize * sizeof(ULONG);

  // Alloc color table
  pColors32 = (ULONG*) AllocVec(colorArrayByteSize, MEMF_PUBLIC|MEMF_CLEAR);
  if(pColors32 == NULL)
  {
    return NULL;
  }

  // Copy starting part of the src color map to dest
  CopyMem((APTR)pSrcBitMapColors32, pColors32, colorArrayByteSize);

  // LoadRGB32() needs the number of colors to load in the higword
  // (the left 16 bit) of the color table's first ULONG value
  pColors32[0] = numColors << 16;

  // Finalize the color array
  pColors32[colorArraySize-1] = 0ul;

  return pColors32;
}


void AnimSheetContainer::cleanup()
{
  struct SheetItemNode* pWorkNode = (struct SheetItemNode*)(m_SheetList.lh_Head);
  struct SheetItemNode* pNextNode;
  while((pNextNode = (struct SheetItemNode*)pWorkNode->ld_Node.ln_Succ) != NULL)
  {
    // Store the next node
    pNextNode = (struct SheetItemNode*)(pWorkNode->ld_Node.ln_Succ);

    // Free all allocations of this node
    if(pWorkNode->pBitMap != NULL)
    {
      FreeBitMap(pWorkNode->pBitMap);
      pWorkNode->pBitMap = NULL;
    }
    
    if(pWorkNode->ld_Node.ln_Name != NULL)
    {
      FreeVec(pWorkNode->ld_Node.ln_Name);
      pWorkNode->ld_Node.ln_Name = NULL;
    }


    FreeVec(pWorkNode);

    // Stored nextNode will be next workNode
    pWorkNode = pNextNode;
  }

  if(m_pColors32 != NULL)
  {
    FreeVec(m_pColors32);
  }

  m_NumSheets = 0;
}
