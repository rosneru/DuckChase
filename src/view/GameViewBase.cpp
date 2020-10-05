#include <stdlib.h>

#include "clib/exec_protos.h"
#include "clib/graphics_protos.h"
#include "GameViewBase.h"

GameViewBase::GameViewBase(OpenIlbmPictureBitMap& picture, 
                           bool allocBitMap)
  : m_BorderTop(0),
    m_BorderLeft(0),
    m_BorderBottom(picture.Height() - 1),
    m_BorderRight(picture.Width() - 1),
    m_IsSafeToChange(true),
    m_IsSafeToWrite(true),
    m_CurrentBuf(1),
    m_pSafePort(NULL),
    m_pDispPort(NULL),
    m_ppBitMapArray(),  // all pointers in array initialized to NULL
    m_Width(picture.Width()),
    m_WordWidth(picture.WordWidth()),
    m_Height(picture.Height()),
    m_Depth(picture.Depth())
{
  //
  // Create the MsgPorts needed for double buffering
  //
  m_pSafePort = CreateMsgPort();
  m_pDispPort = CreateMsgPort();

  if(m_pSafePort == NULL || m_pDispPort == NULL )
  {
    throw "GameViewBase failed to create MsgPort.";
  }


  if(!allocBitMap)
  {
    return;
  }

  //
  // Create and initialize two BitMaps for double buffering
  //
  for(size_t i = 0; i < 2; i++)
  {
    m_ppBitMapArray[i] = AllocBitMap(picture.Width(), 
                                    picture.Height(),
                                    picture.Depth(),
                                    BMF_DISPLAYABLE | BMF_INTERLEAVED | BMF_CLEAR,
                                    NULL);

    if(m_ppBitMapArray[i] == NULL)
    {
      throw "GameViewBase failed to AllocBitMap.";
    }

    if (!(GetBitMapAttr(m_ppBitMapArray[i], BMA_FLAGS) & BMF_INTERLEAVED))
    {
      throw "GameViewBase failed to get a interleaved BitMap";
    }

    WaitBlit();

    // Blit the background image into BitMap i
    BltBitMap(picture.GetBitMap(),
              0,
              0,
              m_ppBitMapArray[i],
              0,
              0,
              picture.Width(),
              picture.Height(),
              0xC0,
              0xFF,
              NULL);
  }
}


GameViewBase::~GameViewBase()
{
  //  Free the double buffer BitMaps
  for(size_t i = 0; i < 2; i++)
  {
    if(m_ppBitMapArray[i] != NULL)
    {
      FreeBitMap(m_ppBitMapArray[i]);
      m_ppBitMapArray[i] = NULL;
    }
  }

  if(m_pSafePort != NULL)
  {
    DeleteMsgPort(m_pSafePort);
  }

  if(m_pDispPort != NULL)
  {
    DeleteMsgPort(m_pDispPort);
  }

}


void GameViewBase::BlitPicture(const OpenIlbmPictureBitMap& picture, 
                               ULONG left, 
                               ULONG top)
{
  // Blit the winner pic into both screen buffers
  for(size_t i = 0; i < 2; i++)
  {
    BltBitMapRastPort(picture.GetBitMap(),
          0,
          0,
          RastPort(),
          left,
          top,
          picture.Width(),
          picture.Height(),
          0xC0);

    WaitBlit();

    // Toggle double buffers
    Render();
  }
}

void GameViewBase::BlitPicture(const OpenIlbmPictureBitMap& picture)
{
  BlitPicture(picture,
              (Width() - picture.Width()) / 2,
              (Height() - picture.Height()) / 2);
}

void GameViewBase::BlitPictureMasked(OpenIlbmPictureBitMap& picture, 
                                     ULONG left, 
                                     ULONG top)
{
  const struct BitMap* pMask = picture.GetMask();
  if(pMask == NULL)
  {
    return;
  }


  // Blit the winner pic into both screen buffers
  for(size_t i = 0; i < 2; i++)
  {
    BltMaskBitMapRastPort(picture.GetBitMap(),
                          0,
                          0,
                          RastPort(),
                          left,
                          top,
                          picture.Width(),
                          picture.Height(),
                          0xe0,
                          pMask->Planes[0]);

    WaitBlit();

    // Toggle double buffers
    Render();
  }
}

void GameViewBase::BlitPictureMasked(OpenIlbmPictureBitMap& picture)
{
  BlitPictureMasked(picture,
                    (Width() - picture.Width()) / 2,
                    (Height() - picture.Height()) / 2);
}




long GameViewBase::Width() const
{
  return m_Width;
}

long GameViewBase::WordWidth() const
{
  return m_WordWidth;
}

long GameViewBase::Height() const
{
  return m_Height;
}


long GameViewBase::Depth() const
{
  return m_Depth;
}


long GameViewBase::BorderTop() const
{
  return m_BorderTop;
}

long GameViewBase::BorderLeft() const
{
  return m_BorderLeft;
}

long GameViewBase::BorderBottom() const
{
  return m_BorderBottom;
}

long GameViewBase::BorderRight() const
{
  return m_BorderRight;
}

void GameViewBase::SetPlayfieldBorders(long top, 
                                       long left, 
                                       long bottom, 
                                       long right)
{
  m_BorderTop = top;
  m_BorderLeft = left;
  m_BorderBottom = bottom;
  m_BorderRight = right;
}
