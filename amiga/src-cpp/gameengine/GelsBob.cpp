#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>

#include <graphics/gels.h>

#include "animtools_proto.h"
#include "GelsBob.h"

GelsBob::GelsBob(short p_pViewDepth,
                 int p_ImageWidth,
                 int p_ImageHeight,
                 short p_ImageDepth)
    : m_ImageWidth(p_ImageWidth),
      m_ImageHeight(p_ImageHeight),
      m_pBob(NULL),
      m_pRastPort(NULL),
      m_pImageShadow(NULL),
      m_CurrentImageIndex(-1),
      m_bIsVisible(false)
{
  // Zeroing all image ptrs of this bob
  for(int i = 0; i < MAX_IMAGES; i++)
  {
    m_ppImagesArray[i] = NULL;
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
  m_NewBob.nb_RasDepth = p_pViewDepth;          // Depth of the raster
  m_NewBob.nb_X = 0;                            // Initial x position
  m_NewBob.nb_Y = 0;                            // Initial y position
  m_NewBob.nb_HitMask = 0;                      // Hit mask
  m_NewBob.nb_MeMask = 0;                       // Me mask
}

GelsBob::~GelsBob()
{
  SetInvisible();
  clear();
}


bool GelsBob::AddRawImage(const char *p_pPath)
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
/*
      // Create shadow mask (or'ing all bits of all bitplanes)
      m_pImageShadow = (WORD*) AllocVec(m_NewBob.nb_WordWidth * 2
                                          * m_NewBob.nb_LineHeight,
                                        MEMF_CHIP);

      int rasSize = m_NewBob.nb_WordWidth * 2 * m_NewBob.nb_LineHeight;

      for(int i = 0; i < m_NewBob.nb_LineHeight; i++)
      {
        for(int j = 0; j < m_NewBob.nb_WordWidth; j++)
        {
          WORD word = 0;
          int idx = i * m_NewBob.nb_WordWidth + j;

          for(int k = 0; k < m_NewBob.nb_ImageDepth; k++)
          {
            word |= m_NewBob.nb_Image[idx + (k * rasSize)];
          }

          m_pImageShadow[idx] = word;
        }
      }

      // Set the shadow mask to the bob
      m_pBob->ImageShadow = m_pImageShadow;
*/
    }
  }

  return m_pBob;
}


void GelsBob::AddToRastPort(struct RastPort* pRastPort)
{
  if(m_pRastPort != NULL)
  {
    // RastPort already set
    return;
  }

  Get();

  if(m_pBob == NULL)
  {
    // No bob to set into RastPort
    return;
  }

  m_pRastPort = pRastPort;
  AddBob(m_pBob, pRastPort);
  m_bIsVisible = true;
}


int GelsBob::XPos() const
{
  if(m_pBob == NULL)
  {
    return 0;
  }

  return m_pBob->BobVSprite->X;
}


int GelsBob::YPos() const
{
  if(m_pBob == NULL)
  {
    return 0;
  }

  return m_pBob->BobVSprite->Y;
}

int GelsBob::Width() const
{
  return m_ImageWidth;
}

int GelsBob::Height() const
{
  return m_ImageHeight;
}

void GelsBob::Move(int x, int y)
{
  if(m_pBob == NULL)
  {
    return;
  }

  m_pBob->BobVSprite->X = x;
  m_pBob->BobVSprite->Y = y;
}


void GelsBob::SetInvisible()
{
  if(m_bIsVisible == false)
  {
    // Already invisible
    return;
  }

  if(m_pBob == NULL)
  {
    return;
  }

  RemBob(m_pBob);
  m_bIsVisible = false;
}


void GelsBob::SetVisible()
{
  if(m_bIsVisible == true)
  {
    // Already visible
    return;
  }

  if(m_pBob == NULL)
  {
    return;
  }

  if(m_pRastPort == NULL)
  {
    return;
  }

  AddBob(m_pBob, m_pRastPort);
  m_bIsVisible = true;
}


bool GelsBob::IsVisible() const
{
  return m_bIsVisible;
}


void GelsBob::NextImage()
{
  if(m_bIsVisible == false)
  {
    // Only animate when bob is visible
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

  InitMasks(m_pBob->BobVSprite);
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
