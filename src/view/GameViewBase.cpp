#include <stdlib.h>

#include "clib/exec_protos.h"
#include "clib/graphics_protos.h"
#include "GameViewBase.h"

GameViewBase::GameViewBase(IlbmBitmap& picture)
  : m_pBitMapArray(),
    m_BorderTop(0),
    m_BorderLeft(0),
    m_BorderBottom(picture.Height() - 1),
    m_BorderRight(picture.Width() - 1),
    m_bDBufSafeToChange(true),
    m_bDBufSafeToWrite(true),
    m_CurrentBuf(1),
    m_pSafeMessage(NULL),
    m_pDispMessage(NULL),
    m_Width(picture.Width()),
    m_WordWidth(picture.WordWidth()),
    m_Height(picture.Height()),
    m_Depth(picture.Depth())
{
  //
  // Create and initialize two BitMaps for double buffering
  //
  for(size_t i = 0; i < 2; i++)
  {
    m_pBitMapArray[i] = AllocBitMap(picture.Width(), 
                                    picture.Height(),
                                    picture.Depth(),
                                    BMF_STANDARD | BMF_INTERLEAVED | BMF_CLEAR,
                                    NULL);

    if(m_pBitMapArray[i] == NULL)
    {
      throw "GameViewBase failed to AllocBitMap.";
    }

    if (!(GetBitMapAttr(m_pBitMapArray[i], BMA_FLAGS) & BMF_INTERLEAVED))
    {
      throw "GameViewBase failed to get a interleaved BitMap";
    }

    WaitBlit();

    // Blit the background image into BitMap i
    BltBitMap(picture.GetBitMap(),
              0,
              0,
              m_pBitMapArray[i],
              0,
              0,
              picture.Width(),
              picture.Height(),
              0xC0,
              0xFF,
              NULL);
  }

  //
  // Create the MsgPorts needed for double buffering
  //
  m_pSafeMessage = CreateMsgPort();
  m_pDispMessage = CreateMsgPort();

  if(m_pSafeMessage == NULL || m_pDispMessage == NULL )
  {
    throw "GameViewBase failed to create MsgPort.";
  }
}


GameViewBase::~GameViewBase()
{
  if(m_pSafeMessage != NULL)
  {
    DeleteMsgPort(m_pSafeMessage);
  }

  if(m_pDispMessage != NULL)
  {
    DeleteMsgPort(m_pDispMessage);
  }

  //  Free the double buffer BitMaps
  for(size_t i = 0; i < 2; i++)
  {
    if(m_pBitMapArray[i] != NULL)
    {
      FreeBitMap(m_pBitMapArray[i]);
      m_pBitMapArray[i] = NULL;
    }
  }
}


void GameViewBase::BlitPicture(const IlbmBitmap& picture, 
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

void GameViewBase::BlitPicture(const IlbmBitmap& picture)
{
  BlitPicture(picture,
              (Width() - picture.Width()) / 2,
              (Height() - picture.Height()) / 2);
}

void GameViewBase::BlitPictureMasked(IlbmBitmap& picture, 
                                     ULONG left, 
                                     ULONG top)
{
  const struct BitMap* pMask = picture.CreateBitMapMask();
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

void GameViewBase::BlitPictureMasked(IlbmBitmap& picture)
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
