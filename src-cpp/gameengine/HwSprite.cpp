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
      m_pCurrentSprite(NULL),
      m_NumberOfHwSprite(-1)
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

bool HwSprite::LoadImgFromRawFile(const char *p_pPath)
{
  // This currently is only using sprite / image 0
  // TODO Change this!!

  BPTR fileHandle = Open(p_pPath, MODE_OLDFILE);
  if (fileHandle == NULL)
  {
    return false;
  }

  // Clear formerly used data
  clear();

  // Calculate the size in bytes needed for one bitplane of given
  // dimension
  int planeSize = RASSIZE(m_ImageWidth, m_ImageHeight);
  int bufSize = planeSize * 5; // TODO Change constant 5!!

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

  // TODO Change constant 5!!
  InitBitMap(&bitMap, 5, m_ImageWidth, m_ImageHeight);

  // Set each plane pointer to its dedicated portion of the data
  UBYTE* pPlanePtr = pPlaneMemoryRaw;
  for(int i = 0; i < 5; i++)  // TODO Change constant 5!!
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

  if(m_NumberOfHwSprite >= 0)
  {
    // Hardware sprite already allocated
    return true;
  }

  // Successful loading of the first sprite requires that it also can
  // be allocated from hardware
  m_NumberOfHwSprite = GetExtSprite(m_pSpriteDataArray[idx], TAG_END);
  if(m_NumberOfHwSprite < 0)
  {
    // Hardware didn't give us one
    return false;
  }

  // Now this is the current sprite until the image is changed e.g. by
  // calling NextImage()
  m_pCurrentSprite = m_pSpriteDataArray[idx];

  return true;
}


struct ExtSprite *HwSprite::Get()
{
  return m_pCurrentSprite;
}


void HwSprite::NextImage()
{
  if(m_CurrentImageIndex < 0)
  {
    // No image loaded
    return;
  }
/*
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

  WORD* pImageData = m_pSpriteDataArray[nextIndex];

  // TODO
  //m_pSprite->BobVSprite->ImageData = pImageData;

  m_CurrentImageIndex = nextIndex;
*/
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

  if (m_NumberOfHwSprite < 0)
  {
    FreeSprite(m_NumberOfHwSprite);
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
