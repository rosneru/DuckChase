#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>

#include <graphics/gels.h>

#include "animtools_proto.h"
#include "HwSprite.h"

HwSprite::HwSprite(int p_ImageWidth,
                   int p_ImageHeight)
    : m_ImageWidth(p_ImageWidth),
      m_ImageHeight(p_ImageHeight),
      m_CurrentImageIndex(-1),
      m_pViewPort(NULL),
      m_pCurrentSprite(NULL),
      m_pEmptySprite(NULL),
      m_HwSpriteNumber(-1)
{
  // Zeroing all image ptrs of this sprite
  for(int i = 0; i < MAX_IMAGES; i++)
  {
    m_pSpriteDataArray[i] = NULL;
  }
}

HwSprite::~HwSprite()
{
  clear();
}

bool HwSprite::AddRawImage(const char *p_pPath)
{
  // This currently is only using sprite / image 0
  // TODO Change this!!

  BPTR fileHandle = Open(p_pPath, MODE_OLDFILE);
  if (fileHandle == NULL)
  {
    return false;
  }

  // Calculate the size in bytes needed for one bitplane of given
  // dimension
  int planeSize = RASSIZE(m_ImageWidth, m_ImageHeight);
  int bufSize = planeSize * 2; // TODO Change constant 2!!

  UBYTE* pPlaneMemoryRaw = (UBYTE*) AllocVec(bufSize,
                                             MEMF_CHIP|MEMF_CLEAR);

  if(pPlaneMemoryRaw == NULL)
  {
    // Not enough memory
    Close(fileHandle);
    return false;
  }

  // Read the file data into target bit plane buffer
  if (Read(fileHandle, pPlaneMemoryRaw, bufSize) != bufSize)
  {
    // Error while reading
    Close(fileHandle);
    return false;
  }

  Close(fileHandle);

  struct BitMap bitMap;

  // TODO Change constant 2!!
  InitBitMap(&bitMap, 2, m_ImageWidth, m_ImageHeight);

  // Set each plane pointer to its dedicated portion of the data
  UBYTE* pPlanePtr = pPlaneMemoryRaw;
  for(int i = 0; i < 2; i++)  // TODO Change constant 2!!
  {
    bitMap.Planes[i] = pPlanePtr;
    pPlanePtr += planeSize;
  }

  int idx = getNextFreeSpriteImageIdx();
  if(idx < 0)
  {
    // Image 'queue' is full - all MAX_IMAGES used.
    FreeVec(pPlaneMemoryRaw);
    return false;
  }

  // Now convert BitMap to 'proper' sprite data
  m_pSpriteDataArray[idx] = AllocSpriteData(&bitMap,
                                            SPRITEA_Width, m_ImageWidth,
                                            TAG_END);

  FreeVec(pPlaneMemoryRaw);

  if(m_pSpriteDataArray[idx] == NULL)
  {
    // Couldn't allocate sprite data
    return false;
  }

  if(m_HwSpriteNumber >= 0)
  {
    // Hardware sprite and empty sprite already allocated
    return true;
  }

  // Allocate the empty sprite data (for setting sprite invisible)
  if(m_pEmptySprite == NULL)
  {
    m_pEmptySprite = AllocSpriteData(NULL, TAG_END);
  }

  // Successful loading of the first sprite requires that it also can
  // be allocated from hardware
  m_HwSpriteNumber = GetExtSprite(m_pSpriteDataArray[idx],
                                  TAG_END);   // of spr 0 and 1 fit
                                              // my bitmap colors
                                              // TODO change!!

  if(m_HwSpriteNumber < 0)
  {
    // No hardware sprite available
    return false;
  }

  // Now this is the current sprite until the image is changed e.g. by
  // calling NextImage()
  m_pCurrentSprite = m_pSpriteDataArray[idx];

  // Relatively safe way to use sprite 0 as demonstrated by demo AABoing
  //m_pCurrentSprite->es_SimpleSprite.num = 0;

  m_CurrentImageIndex = 0;

  return true;
}


struct ExtSprite *HwSprite::Get()
{
  return m_pCurrentSprite;
}

int HwSprite::SpriteNumber()
{
  return m_HwSpriteNumber;
}

void HwSprite::SetViewPort(struct ViewPort* pViewPort)
{
  if(m_pViewPort != NULL)
  {
    // RastPort already set
    return;
  }

  m_pViewPort = pViewPort;
}


int HwSprite::XPos() const
{
  if(m_pCurrentSprite == NULL)
  {
    return 0;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*) m_pCurrentSprite;
  return pSpr->x;
}


int HwSprite::YPos() const
{
  if(m_pCurrentSprite == NULL)
  {
    return 0;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*) m_pCurrentSprite;
  return pSpr->y;
}

int HwSprite::Width() const
{
  return m_ImageWidth;
}

int HwSprite::Height() const
{
  return m_ImageHeight;
}


void HwSprite::Move(int x, int y)
{
  if(m_pCurrentSprite == NULL)
  {
    return;
  }

  if(m_pViewPort == NULL)
  {
    return;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*) m_pCurrentSprite;
  MoveSprite(m_pViewPort, pSpr, x, y);
}


void HwSprite::SetInvisible()
{
  if(m_pCurrentSprite == NULL)
  {
    return;
  }

  if(m_pViewPort == NULL)
  {
    return;
  }

  struct ExtSprite* pOldSprite = m_pCurrentSprite;
  m_pCurrentSprite = m_pEmptySprite;
  ChangeExtSprite(m_pViewPort, pOldSprite, m_pCurrentSprite, TAG_END);
}


void HwSprite::SetVisible()
{
  if(m_pCurrentSprite == NULL)
  {
    return;
  }

  if(m_pViewPort == NULL)
  {
    return;
  }

  struct ExtSprite* pOldSprite = m_pCurrentSprite;
  m_pCurrentSprite = m_pSpriteDataArray[m_CurrentImageIndex];
  ChangeExtSprite(m_pViewPort, pOldSprite, m_pCurrentSprite, TAG_END);
}


bool HwSprite::IsVisible() const
{
  return m_pCurrentSprite != m_pEmptySprite;
}


void HwSprite::NextImage()
{
  if(m_pCurrentSprite == m_pEmptySprite)
  {
    // Only animate when sprite is visible
    return;
  }

  if(m_pViewPort == NULL)
  {
    return;
  }

  if(m_CurrentImageIndex < 0)
  {
    // No image loaded
    return;
  }

  // Get the image data for the next image
  int nextIndex = m_CurrentImageIndex + 1;

  if(nextIndex >= MAX_IMAGES)
  {
    nextIndex = 0;
  }

  if(m_pSpriteDataArray[nextIndex] == NULL)
  {
    // No next image available, starting again with the first
    nextIndex = 0;
  }

  if(nextIndex == m_CurrentImageIndex)
  {
    // Nothing has changed
    return;
  }

  struct ExtSprite* pOldSprite = m_pCurrentSprite;
  m_pCurrentSprite = m_pSpriteDataArray[nextIndex];
  m_CurrentImageIndex = nextIndex;

  ChangeExtSprite(m_pViewPort, pOldSprite, m_pCurrentSprite, TAG_END);
}


int HwSprite::getNextFreeSpriteImageIdx()
{
  // Find the next free index in image data array
  int idx = -1;
  for(int i = 0; i < MAX_IMAGES; i++)
  {
    if(m_pSpriteDataArray[i] == NULL)
    {
      idx = i;
      break;
    }
  }

  return idx;
}


void HwSprite::clear()
{

  if (m_HwSpriteNumber >= 0)
  {
    FreeSprite(m_HwSpriteNumber);
  }

  for(int i = 0; i < MAX_IMAGES; i++)
  {
    struct ExtSprite* pSpriteData = m_pSpriteDataArray[i];
    if (pSpriteData != NULL)
    {
      FreeSpriteData(pSpriteData);
    }

    m_pSpriteDataArray[i] = NULL;
  }

}
