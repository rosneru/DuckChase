#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include <graphics/gels.h>

#include "animtools_proto.h"
#include "GelsBob.h"

GelsBob::GelsBob(struct Screen *p_pScreen,
                 int p_ImageWidth,
                 int p_ImageHeight,
                 short p_ImageDepth)
    : m_pScreen(p_pScreen),
      m_pBob(NULL),
      m_pImageShadow(NULL),
      m_CurrentImageIndex(-1)
{
  // Zeroing all image ptrs of this bob
  for(int i = 0; i < MAX_IMAGES; i++)
  {
    m_ppImagesArray[i] = NULL;
  }

  // Getting the depth of the screen
  struct DrawInfo* pDrawInfo = GetScreenDrawInfo(p_pScreen);
  if(pDrawInfo == NULL)
  {
    // Can't initialize without DrawInfo (needed for screen depth)
    return;
  }

  // Determine wordWidth and imageBufSize
  SHORT wordWidth = ((p_ImageWidth + 15) & -16) >> 4;
  m_ImageBufSize = wordWidth * 2 * p_ImageHeight * p_ImageDepth;

  // Fill the NewBob struct
  m_NewBob.nb_Image = NULL;                     // Image data
  m_NewBob.nb_WordWidth = wordWidth;            // Bob width
  m_NewBob.nb_LineHeight = p_ImageHeight;       // Bob height in lines
  m_NewBob.nb_ImageDepth = p_ImageDepth;        // Image depth
  m_NewBob.nb_PlanePick = 7;   // TODO generalize!!             // Planes that get image data
  m_NewBob.nb_PlaneOnOff = 0;                   // Unused planes to turn on
  m_NewBob.nb_BFlags = SAVEBACK | OVERLAY;      // Bog flags
  m_NewBob.nb_DBuf = 1;                         // DoubleBuffering.
  m_NewBob.nb_RasDepth = pDrawInfo->dri_Depth;  // Depth of the raster
  m_NewBob.nb_X = 0;                            // Initial x position
  m_NewBob.nb_Y = 0;                            // Initial y position
  m_NewBob.nb_HitMask = 0;                      // Hit mask
  m_NewBob.nb_MeMask = 0;                       // Me mask

  FreeScreenDrawInfo(p_pScreen, pDrawInfo);

  m_pImageShadow = (WORD*) AllocVec(wordWidth * 2 * p_ImageHeight, MEMF_CHIP|MEMF_CLEAR);
}

GelsBob::~GelsBob()
{
  clear();
}

bool GelsBob::LoadImgFromRawFile(const char *p_pPath)
{
  // Opening the file
  BPTR fileHandle = Open(p_pPath, MODE_OLDFILE);
  if (fileHandle == NULL)
  {
    return false;
  }

  WORD* pImageData = createNextImageData();
  if(pImageData == NULL)
  {
    // Image index full or memory allocation failed
    Close(fileHandle);
    return false;
  }

  // Read the file data into target chip memory buffer
  if (Read(fileHandle, pImageData, m_ImageBufSize) != m_ImageBufSize)
  {
    // Error while reading
    Close(fileHandle);
    return false;
  }

  Close(fileHandle);
  return true;
}

bool GelsBob::LoadImgFromArray(const WORD *p_pAddress)
{
  WORD* pImageData = createNextImageData();
  if(pImageData == NULL)
  {
    // Image index full or memory allocation failed
    return false;
  }

  // Copy source data to target chip memory buffer
  for (int i = 0; i < (m_ImageBufSize / 2); i++)
  {
    pImageData[i] = p_pAddress[i];
  }

  return true;
}

struct Bob *GelsBob::Get()
{
  if (m_pBob == NULL)
  {
    if(m_ppImagesArray[0] == NULL)
    {
      // At least one image must be loaded
      return NULL;
    }

    // Bob doesn't exist - create it using the first image of the image
    // data array pointers
    m_NewBob.nb_Image = m_ppImagesArray[0];

    // Create the Bob
    m_pBob = makeBob(&m_NewBob);

    if(m_pBob != NULL)
    {
      m_CurrentImageIndex = 0;

      // Create shadow mask (or'ing all bits of all bitplanes)
      int rasSize = m_NewBob.nb_WordWidth * 2 * m_NewBob.nb_LineHeight;

      for(int i = 0; i < m_NewBob.nb_WordWidth; i++)
      {
        for(int j = 0; j < m_NewBob.nb_LineHeight; j++)
        {
          WORD word = 0;

          int idx = i * m_NewBob.nb_LineHeight
                  + j * m_NewBob.nb_ImageDepth;

          for(int k = 0; m_NewBob.nb_ImageDepth; k++)
          {
            word |= m_NewBob.nb_Image[idx + k * rasSize];
          }

          m_pImageShadow[idx] = word;
        }
      }

      // Set the shadow mask to the bob
      m_pBob->ImageShadow = m_pImageShadow;
    }
  }

  return m_pBob;
}


void GelsBob::NextImage()
{
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

  if(m_ppImagesArray[nextIndex] == NULL)
  {
    // No next image available, starting again with the first
    nextIndex = 0;
  }

  if(nextIndex == m_CurrentImageIndex)
  {
    // Nothing has changed
    return;
  }

  WORD* pImageData = m_ppImagesArray[nextIndex];

  m_pBob->BobVSprite->ImageData = pImageData;

  m_CurrentImageIndex = nextIndex;
}


WORD* GelsBob::createNextImageData()
{
  // Find the next free index in image data array
  int idx = -1;
  for(int i = 0; i < MAX_IMAGES; i++)
  {
    if(m_ppImagesArray[i] == NULL)
    {
      idx = i;
      break;
    }
  }

  if(idx == -1)
  {
    // No free image data index found
    return NULL;
  }

  // Allocate chip memory
  m_ppImagesArray[idx] = (WORD*) AllocVec(m_ImageBufSize,
                                          MEMF_CHIP|MEMF_CLEAR);

  return m_ppImagesArray[idx];
}

void GelsBob::clear()
{
  if (m_pBob != NULL)
  {
    freeBob(m_pBob, m_NewBob.nb_RasDepth);
    m_pBob = NULL;
  }

  if (m_pImageShadow != NULL)
  {
    FreeVec(m_pImageShadow);
  }


  for(int i = 0; i < MAX_IMAGES; i++)
  {
    WORD* pImageData = m_ppImagesArray[i];
    if (pImageData != NULL)
    {
      FreeVec(pImageData);
    }

    m_ppImagesArray[i] = NULL;
  }
}
