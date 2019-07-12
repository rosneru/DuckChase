#include <proto/exec.h>
#include <proto/dos.h>

#include <graphics/gels.h>

#include "animtools_proto.h"
#include "GelsBob.h"

GelsBob::GelsBob(struct Screen *p_pScreen,
                 struct Window* p_pWindow,
                 short p_ScreenDepth,
                 int p_ImageWidth,
                 int p_ImageHeight,
                 short p_ImageDepth)
    : m_pScreen(p_pScreen),
      m_pWindow(p_pWindow),
      m_pBob(NULL),
      m_CurrentImageIndex(-1)
{
  //
  for(int i = 0; i < MAX_IMAGES; i++)
  {
    m_ppImagesArray[i] = NULL;
  }

  SHORT wordWidth = ((p_ImageWidth + 15) & -16) >> 4;

  m_ImageBufSize = wordWidth * p_ImageHeight * p_ImageDepth * 2;

  m_NewBob.nb_Image = NULL;                // Image data
  m_NewBob.nb_WordWidth = wordWidth;       // Bob width
  m_NewBob.nb_LineHeight = p_ImageHeight;  // Bob height in lines
  m_NewBob.nb_ImageDepth = p_ImageDepth;   // Image depth
  m_NewBob.nb_PlanePick = 7;   // TODO generalize!!             // Planes that get image data
  m_NewBob.nb_PlaneOnOff = 0;              // Unused planes to turn on
  m_NewBob.nb_BFlags = SAVEBACK | OVERLAY; // Bog flags
  m_NewBob.nb_DBuf = 0;                    // DoubleBuffering.
  m_NewBob.nb_RasDepth = p_ScreenDepth;    // Depth of the raster
  m_NewBob.nb_X = 0;                       // Initial x position
  m_NewBob.nb_Y = 0;                       // Initial y position
  m_NewBob.nb_HitMask = 0;                 // Hit mask
  m_NewBob.nb_MeMask = 0;                  // Me mask
}

GelsBob::~GelsBob()
{
  clear();
}

bool GelsBob::LoadImgFromRawFile(const char *p_pPath)
{
  //
  //  Open the file and calculate its sie using Seek()
  //
  BPTR fileHandle = Open(p_pPath, MODE_OLDFILE);
  if (fileHandle == NULL)
  {
    return false;
  }

  WORD* pImageData = createNextImageData();
  if(pImageData == NULL)
  {
    // Image index full or memory allocation failed
    return false;
  }


  //
  // Read the file data into target chip memory buffer
  //
  if (Read(fileHandle, pImageData, m_ImageBufSize) != m_ImageBufSize)
  {
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

  //
  // Copy source data to target chip memory buffer
  //
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

    //
    // Finally create the Bob
    //
    m_pBob = makeBob(&m_NewBob);

    if(m_pBob != NULL)
    {
      m_CurrentImageIndex = 0;
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
