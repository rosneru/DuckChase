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
    if(addItemNode(pic.GetBitMap(), 0) == false)
    {
      throw "Failed to create node for ILBM sheet.\n";
    }

    m_pColors32 = deepCopyColors(pic.GetColors32(), pic.Depth());
    return;
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

    struct BitMap* pBitMap;
    ULONG idx = 0;
    while((pBitMap = pic.parseNextAnimSheet()) != NULL)
    {
      addItemNode(pBitMap, idx);
      idx++;
    }
    
    // Deep copy 32 colors (depth = 5) because AMOS sprite bank depth is
    // always = 5. 
    m_pColors32 = deepCopyColors(pic.parseColors32(), 5);
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
  struct Node* pNode;

  if(index >= m_NumSheets)
  { 
    // Wanted index is outside list bounds
    return NULL;
  }

  // Point pNode to the header node (which contains no data)
  pNode = m_SheetList.lh_Head;
  if(pNode == NULL)
  {
    return NULL;
  }

  do
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
  while((pNode = pNode->ln_Succ) != NULL);  // As long there is a successor

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

bool AnimSheetContainer::addItemNode(const struct BitMap* pBitMap, 
                                     ULONG initialIndex)
{
  struct SheetItemNode* pItemNode;
  ULONG width, height, depth;

  if(pBitMap == NULL)
  {
    return false;
  }

  width = GetBitMapAttr(pBitMap, BMA_WIDTH);
  height = GetBitMapAttr(pBitMap, BMA_HEIGHT);
  depth = GetBitMapAttr(pBitMap, BMA_DEPTH);

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
          width, 
          height, 
          depth);
  
  pItemNode->pBitMap = AllocBitMap(width,
                                   height,
                                   depth,
                                   BMF_CLEAR,
                                   pBitMap);

  if(pItemNode->pBitMap == NULL)
  {
    // Failed to allocate BitMap for this sheet
    FreeVec(pItemNode->ld_Node.ln_Name);
    FreeVec(pItemNode);
    return false;
  }

  BltBitMap(pBitMap,
            0, 0,
            pItemNode->pBitMap,
            0, 0, 
            width, height,
            0Xc0,
            0xff,
            NULL);

  pItemNode->Width = width;
  pItemNode->Height = height;
  pItemNode->Depth = depth;

  AddTail(&m_SheetList, (struct Node*)pItemNode);
  m_NumSheets++;
  return true;
}


ULONG* AnimSheetContainer::deepCopyColors(ULONG* pSrcColors32, ULONG srcDepth)
{
  ULONG numColors, colorArraySize, colorArrayByteSize;
  ULONG* pDstColors32;

  // Copy the first n colors from source image (with n = depth)
  if(pSrcColors32 == NULL)
  {
    return NULL;
  }

  numColors = 1L << srcDepth;

  // Size of the Colors32 table (number of ULONG values)
  colorArraySize = 3 * numColors + 2;

  // Size of the Colors32 table in bytes
  colorArrayByteSize = colorArraySize * sizeof(ULONG);

  // Alloc color table
  pDstColors32 = (ULONG*) AllocVec(colorArrayByteSize, MEMF_PUBLIC|MEMF_CLEAR);
  if(pDstColors32 == NULL)
  {
    return NULL;
  }

  // Copy starting part of the src color map to dest
  CopyMem((APTR)pSrcColors32, pDstColors32, colorArrayByteSize);

  // LoadRGB32() needs the number of colors to load in the higword
  // (the left 16 bit) of the color table's first ULONG value
  pDstColors32[0] = numColors << 16;

  // Finalize the color array
  pDstColors32[colorArraySize-1] = 0ul;

  return pDstColors32;
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
