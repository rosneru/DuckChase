#include <stdio.h>

#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

#include <graphics/gels.h>

#include "ShapeBob.h"


ShapeBob::ShapeBob(struct RastPort* pRastPort, 
                   int rasterDepth,
                   const GfxResourceGELs& gfxResources)
  : ShapeBase(gfxResources.Width(), 
              gfxResources.Height(), 
              gfxResources.Depth()),
    m_pRastPort(pRastPort),
    m_RasterDepth(rasterDepth),
    m_pBob(NULL),
    m_bIsVisible(false),
    m_RasSize(sizeof(UWORD) * m_WordWidth * m_Height * m_RasterDepth),
    m_LineSize(sizeof(WORD) * m_WordWidth),
    m_PlaneSize(m_LineSize * m_Height)
{
  m_pBob = (struct Bob*) AllocVec(sizeof(struct Bob), MEMF_CLEAR);
  if(m_pBob == NULL)
  {
    throw "ShapeBob: Failed to allocate memory for bob.";
  }

  m_pBob->SaveBuffer = (WORD*) AllocVec(m_RasSize, MEMF_CHIP);
  if(m_pBob->SaveBuffer == NULL)
  {
    throw "ShapeBob: Failed to allocate memory for save buffer.";
  }

  struct VSprite* pVSprite; 
  pVSprite = (struct VSprite*) AllocVec(sizeof(struct VSprite), MEMF_CLEAR);
  if(pVSprite == NULL)
  {
    throw "ShapeBob: Failed to allocate memory for vsprite.";
  }

  pVSprite->BorderLine = (WORD*) AllocVec(m_LineSize, MEMF_CHIP);
  if(pVSprite->BorderLine == NULL)
  {
    throw "ShapeBob: Failed to allocate memory for border line.";
  }

  pVSprite->CollMask = (WORD*) AllocVec(m_PlaneSize, MEMF_CHIP);
  if(pVSprite->CollMask == NULL)
  {
    throw "ShapeBob: Failed to allocate memory for collision mask.";
  }

  pVSprite->X = 0;
  pVSprite->Y = 0;
  pVSprite->Flags = SAVEBACK | OVERLAY;
  pVSprite->Width = m_WordWidth;
  pVSprite->Depth = m_Depth;
  pVSprite->Height = m_Height;
  pVSprite->MeMask = 0;
  pVSprite->HitMask = 0;
  pVSprite->ImageData = gfxResources.DefaultImage();
  pVSprite->SprColors = NULL;
  pVSprite->PlanePick  = 0x0; // Will be set to proper values after VSprite creation
  pVSprite->PlaneOnOff = 0x0;
  InitMasks(pVSprite);

  // For bobs these two properties should be set after VSprite creation
  pVSprite->PlanePick  = 0xf; // TODO generalize!! (here: planes 0..3 are selected to contain image data)
  pVSprite->PlaneOnOff = 0x0;
  
  // Link bob and VSprite
  pVSprite->VSBob = m_pBob;
  m_pBob->BobVSprite = pVSprite;
  m_pBob->ImageShadow = pVSprite->CollMask;

  // Clear some values
  m_pBob->Flags = 0;
  m_pBob->Before = NULL;
  m_pBob->After = NULL;
  m_pBob->BobComp = NULL;

  bool isDoubleBuffered = true;
  if(isDoubleBuffered)
  {
    m_pBob->DBuffer = (struct DBufPacket *) AllocVec(sizeof(struct DBufPacket), 
                                                     MEMF_CLEAR);
    if(m_pBob->DBuffer == NULL)                   
    {
      throw "ShapeBob: Failed to allocate memory for DBufPacket.";
    }

    m_pBob->DBuffer->BufBuffer = (WORD*) AllocVec(m_RasSize, MEMF_CHIP);
    if(m_pBob->DBuffer->BufBuffer == NULL)
    {
      throw "ShapeBob: Failed to allocate memory for double buffer.";
    }
  }

  AddBob(m_pBob, m_pRastPort);
  m_bIsVisible = true;
}


ShapeBob::~ShapeBob()
{
  if (m_pBob != NULL)
  {
    if(m_pBob->DBuffer != NULL)
    {
      if(m_pBob->DBuffer->BufBuffer != NULL)
      {
        FreeVec(m_pBob->DBuffer->BufBuffer);
        m_pBob->DBuffer->BufBuffer = NULL;
      }

      FreeVec(m_pBob->DBuffer);
      m_pBob->DBuffer = NULL;
    }

    if(m_pBob->BobVSprite != NULL)
    {
      if(m_pBob->BobVSprite->BorderLine != NULL)
      {
        FreeVec(m_pBob->BobVSprite->BorderLine);
        m_pBob->BobVSprite->BorderLine = NULL;
      }

      if(m_pBob->BobVSprite->CollMask != NULL)
      {
        FreeVec(m_pBob->BobVSprite->CollMask);
        m_pBob->BobVSprite->CollMask = NULL;
      }

      FreeVec(m_pBob->BobVSprite);
      m_pBob->BobVSprite = NULL;
    }
    
    if(m_pBob->SaveBuffer != NULL)
    {
      FreeVec(m_pBob->SaveBuffer);
      m_pBob->SaveBuffer = NULL;
    }

    FreeVec(m_pBob);
    m_pBob = NULL;
  }
}



int ShapeBob::Left() const
{
  if (m_pBob == NULL)
  {
    return 0;
  }

  return m_pBob->BobVSprite->X;
}

int ShapeBob::Top() const
{
  if (m_pBob == NULL)
  {
    return 0;
  }

  return m_pBob->BobVSprite->Y;
}

int ShapeBob::Right() const
{
  if (m_pBob == NULL)
  {
    return 0;
  }

  return m_pBob->BobVSprite->X + Width();
}

int ShapeBob::Bottom() const
{
  if (m_pBob == NULL)
  {
    return 0;
  }

  return m_pBob->BobVSprite->Y + Height();
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

bool ShapeBob::IsGone() const
{
  if(m_pBob == NULL)
  {
    return false;
  }

  return (m_pBob->BobVSprite->Flags & GELGONE) != 0;
}


void ShapeBob::SetImage(WORD* pNewImage)
{
  m_pBob->BobVSprite->ImageData = pNewImage;
  InitMasks(m_pBob->BobVSprite);
}

void ShapeBob::move(int x, int y)
{
  if (m_pBob == NULL)
  {
    return;
  }

  m_pBob->BobVSprite->X = x;
  m_pBob->BobVSprite->Y = y;
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

