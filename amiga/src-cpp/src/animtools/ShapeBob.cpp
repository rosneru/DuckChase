#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>

#include <graphics/gels.h>

#include "ShapeBob.h"
#include "animtools_proto.h"

ShapeBob::ShapeBob(int rasterDepth)
  : m_RasterDepth(rasterDepth),
    m_pBob(NULL),
    m_pAnimSeq(NULL),
    m_pRastPort(NULL),
    m_bIsVisible(false)
{

}

ShapeBob::~ShapeBob()
{
  SetInvisible();
  
  if (m_pBob != NULL)
  {
    freeBob(m_pBob, m_RasterDepth);
    m_pBob = NULL;
  }
}

void ShapeBob::createBob()
{
  if (m_pBob != NULL)
  {
    return;
  }

  if (m_pAnimSeq == NULL)
  {
    // An anim must have been added
    return;
  }

  WORD* pImageData = m_pAnimSeq->GetFirstImage();
  if (pImageData == NULL)
  {
    // At least one image must be loaded
    return;
  }

  // Create the bob using the first image
  // Fill the NewBob struct
  NEWBOB newBob;
  newBob.nb_Image = pImageData;                     // Img data
  newBob.nb_WordWidth = m_pAnimSeq->GetWordWidth(); // Img width in words
  newBob.nb_LineHeight = m_pAnimSeq->GetHeight();   // Img height in lines
  newBob.nb_ImageDepth = m_pAnimSeq->GetDepth();    // Img depth
  newBob.nb_PlanePick = 7;                          // Planes that get image data // TODO generalize!!
  newBob.nb_PlaneOnOff = 0;              // Don't turn on unused planes
  newBob.nb_BFlags = SAVEBACK | OVERLAY; // Bog flags
  newBob.nb_DBuf = 1;                    // DoubleBuffering.
  newBob.nb_RasDepth = m_RasterDepth;    // Depth of the raster
  newBob.nb_X = 0;                       // Initial x position
  newBob.nb_Y = 0;                       // Initial y position
  newBob.nb_HitMask = 0;                 // Hit mask
  newBob.nb_MeMask = 0;                  // Me mask

  // Create the Bob
  m_pBob = makeBob(&newBob);
}

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

void ShapeBob::AddToRastPort(struct RastPort* pRastPort)
{
  if (m_pRastPort != NULL)
  {
    // RastPort already set
    return;
  }

  createBob();

  if (m_pBob == NULL)
  {
    // No bob to set into RastPort
    return;
  }

  m_pRastPort = pRastPort;
  AddBob(m_pBob, pRastPort);
  m_bIsVisible = true;
}

int ShapeBob::XPos() const
{
  if (m_pBob == NULL)
  {
    return 0;
  }

  return m_pBob->BobVSprite->X;
}

int ShapeBob::YPos() const
{
  if (m_pBob == NULL)
  {
    return 0;
  }

  return m_pBob->BobVSprite->Y;
}

int ShapeBob::Width() const
{
  if(m_pAnimSeq == NULL)
  {
    return 0;
  }

  return m_pAnimSeq->GetWidth();
}

int ShapeBob::Height() const
{
  if(m_pAnimSeq == NULL)
  {
    return 0;
  }

  return m_pAnimSeq->GetHeight();
}

void ShapeBob::Move(int x, int y)
{
  if (m_pBob == NULL)
  {
    return;
  }

  m_pBob->BobVSprite->X = x;
  m_pBob->BobVSprite->Y = y;
}

void ShapeBob::SetInvisible()
{
  if (m_bIsVisible == false)
  {
    // Already invisible
    return;
  }

  if (m_pBob == NULL)
  {
    return;
  }

  RemBob(m_pBob);
  m_bIsVisible = false;
}

void ShapeBob::SetVisible()
{
  if (m_bIsVisible == true)
  {
    // Already visible
    return;
  }

  if (m_pBob == NULL)
  {
    return;
  }

  if (m_pRastPort == NULL)
  {
    return;
  }

  AddBob(m_pBob, m_pRastPort);
  m_bIsVisible = true;
}

bool ShapeBob::IsVisible() const
{
  return m_bIsVisible;
}

void ShapeBob::SetAnimSequence(AnimSeqBase* pAnimSequence)
{
  if(pAnimSequence == NULL)
  {
    return;
  }

  m_pAnimSeq = static_cast<AnimSeqBob*>(pAnimSequence);

  if (m_pBob == NULL)
  {
    // If no bob object exists, create it
    createBob();
  }
}

void ShapeBob::NextImage()
{
  if (m_bIsVisible == false)
  {
    // Only animate when bob is visible
    return;
  }

  if (m_pAnimSeq == NULL)
  {
    return;
  }

  WORD* pNextImg = m_pAnimSeq->GetNextImage();
  if (pNextImg == NULL)
  {
    return;
  }

  m_pBob->BobVSprite->ImageData = pNextImg;
  InitMasks(m_pBob->BobVSprite);
}
