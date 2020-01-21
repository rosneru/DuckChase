#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>

#include <graphics/gels.h>

#include "ShapeSprite.h"
#include "animtools_proto.h"

ShapeSprite::ShapeSprite(int p_ImageWidth, int p_ImageHeight)
  : m_ImageWidth(p_ImageWidth),
    m_ImageHeight(p_ImageHeight),
    m_MaxImages(8),
    m_CurrentImageIndex(-1),
    m_pViewPort(NULL),
    m_pSpriteDataArray(NULL),
    m_pCurrentSprite(NULL),
    m_pEmptySprite(NULL),
    m_HwSpriteNumber(-1)
{
}

ShapeSprite::~ShapeSprite()
{
  clear();
}

bool ShapeSprite::AddRawImage(const char* p_pPath)
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

  UBYTE* pPlaneMemoryRaw = (UBYTE*)AllocVec(bufSize, MEMF_CHIP | MEMF_CLEAR);

  if (pPlaneMemoryRaw == NULL)
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
  for (int i = 0; i < 2; i++) // TODO Change constant 2!!
  {
    bitMap.Planes[i] = pPlanePtr;
    pPlanePtr += planeSize;
  }

  int idx = getNextFreeSpriteImageIdx();
  if (idx < 0)
  {
    // Image 'queue' is full - all m_MaxImages used.
    FreeVec(pPlaneMemoryRaw);
    return false;
  }

  // Now convert BitMap to 'proper' sprite data
  m_pSpriteDataArray[idx] =
    AllocSpriteData(&bitMap, SPRITEA_Width, m_ImageWidth, TAG_END);

  FreeVec(pPlaneMemoryRaw);

  if (m_pSpriteDataArray[idx] == NULL)
  {
    // Couldn't allocate sprite data
    return false;
  }

  if (m_HwSpriteNumber >= 0)
  {
    // Hardware sprite and empty sprite already allocated
    return true;
  }

  // Allocate the empty sprite data (for setting sprite invisible)
  if (m_pEmptySprite == NULL)
  {
    m_pEmptySprite = AllocSpriteData(NULL, TAG_END);
  }

  // Successful loading of the first sprite requires that it also can
  // be allocated from hardware
  m_HwSpriteNumber = GetExtSprite(m_pSpriteDataArray[idx],
                                  TAG_END); // of spr 0 and 1 fit
                                            // my bitmap colors
                                            // TODO change!!

  if (m_HwSpriteNumber < 0)
  {
    // No hardware sprite available
    return false;
  }

  // Now this is the current sprite until the image is changed e.g. by
  // calling NextImage()
  m_pCurrentSprite = m_pSpriteDataArray[idx];

  // Relatively safe way to use sprite 0 as demonstrated by demo AABoing
  // m_pCurrentSprite->es_SimpleSprite.num = 0;

  m_CurrentImageIndex = 0;

  return true;
}

struct ExtSprite* ShapeSprite::Get()
{
  return m_pCurrentSprite;
}

int ShapeSprite::SpriteNumber()
{
  return m_HwSpriteNumber;
}

void ShapeSprite::SetViewPort(struct ViewPort* pViewPort)
{
  if (m_pViewPort != NULL)
  {
    // RastPort already set
    return;
  }

  m_pViewPort = pViewPort;
}

int ShapeSprite::XPos() const
{
  if (m_pCurrentSprite == NULL)
  {
    return 0;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*)m_pCurrentSprite;
  return pSpr->x;
}

int ShapeSprite::YPos() const
{
  if (m_pCurrentSprite == NULL)
  {
    return 0;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*)m_pCurrentSprite;
  return pSpr->y;
}

int ShapeSprite::Width() const
{
  return m_ImageWidth;
}

int ShapeSprite::Height() const
{
  return m_ImageHeight;
}

void ShapeSprite::Move(int x, int y)
{
  if (m_pCurrentSprite == NULL)
  {
    return;
  }

  if (m_pViewPort == NULL)
  {
    return;
  }

  struct SimpleSprite* pSpr = (struct SimpleSprite*)m_pCurrentSprite;
  MoveSprite(m_pViewPort, pSpr, x, y);
}

void ShapeSprite::SetInvisible()
{
  if (m_pCurrentSprite == NULL)
  {
    return;
  }

  if (m_pViewPort == NULL)
  {
    return;
  }

  struct ExtSprite* pOldSprite = m_pCurrentSprite;
  m_pCurrentSprite = m_pEmptySprite;
  ChangeExtSprite(m_pViewPort, pOldSprite, m_pCurrentSprite, TAG_END);
}

void ShapeSprite::SetVisible()
{
  if (m_pCurrentSprite == NULL)
  {
    return;
  }

  if (m_pViewPort == NULL)
  {
    return;
  }

  struct ExtSprite* pOldSprite = m_pCurrentSprite;
  m_pCurrentSprite = m_pSpriteDataArray[m_CurrentImageIndex];
  ChangeExtSprite(m_pViewPort, pOldSprite, m_pCurrentSprite, TAG_END);
}

bool ShapeSprite::IsVisible() const
{
  return m_pCurrentSprite != m_pEmptySprite;
}

void ShapeSprite::NextImage()
{
  if (m_pCurrentSprite == m_pEmptySprite)
  {
    // Only animate when sprite is visible
    return;
  }

  if (m_pViewPort == NULL)
  {
    return;
  }

  if (m_CurrentImageIndex < 0)
  {
    // No image loaded
    return;
  }

  // Get the image data for the next image
  int nextIndex = m_CurrentImageIndex + 1;

  if (nextIndex >= m_MaxImages)
  {
    nextIndex = 0;
  }

  if (m_pSpriteDataArray[nextIndex] == NULL)
  {
    // No next image available, starting again with the first
    nextIndex = 0;
  }

  if (nextIndex == m_CurrentImageIndex)
  {
    // Nothing has changed
    return;
  }

  struct ExtSprite* pOldSprite = m_pCurrentSprite;
  m_pCurrentSprite = m_pSpriteDataArray[nextIndex];
  m_CurrentImageIndex = nextIndex;

  ChangeExtSprite(m_pViewPort, pOldSprite, m_pCurrentSprite, TAG_END);
}

int ShapeSprite::getNextFreeSpriteImageIdx()
{
  if (m_pSpriteDataArray == NULL)
  {
    m_pSpriteDataArray =
      (struct ExtSprite**)AllocVec(sizeof(struct ExtSprite*), MEMF_CLEAR);
  }

  // Find the next free index in image data array
  int idx = -1;
  for (int i = 0; i < m_MaxImages; i++)
  {
    if (m_pSpriteDataArray[i] == NULL)
    {
      idx = i;
      break;
    }
  }

  return idx;
}

void ShapeSprite::clear()
{
  if (m_HwSpriteNumber >= 0)
  {
    FreeSprite(m_HwSpriteNumber);
  }

  if (m_pSpriteDataArray == NULL)
  {
    return;
  }

  for (int i = 0; i < m_MaxImages; i++)
  {
    struct ExtSprite* pSpriteData = m_pSpriteDataArray[i];
    if (pSpriteData != NULL)
    {
      FreeSpriteData(pSpriteData);
    }

    m_pSpriteDataArray[i] = NULL;
  }

  FreeVec(m_pSpriteDataArray);
  m_pSpriteDataArray = NULL;
}
