#include <stdlib.h>

#include "clib/exec_protos.h"
#include "clib/graphics_protos.h"
#include "GameViewBase.h"

GameViewBase::GameViewBase(IlbmBitmap& backgroundPicture)
  : m_pBitMapArray(),
    m_BorderTop(0),
    m_BorderLeft(0),
    m_BorderBottom(backgroundPicture.Height() - 1),
    m_BorderRight(backgroundPicture.Width() - 1),
    m_bDBufSafeToChange(true),
    m_bDBufSafeToWrite(true),
    m_CurrentBuf(1),
    m_pSafeMessage(NULL),
    m_pDispMessage(NULL),
    m_Width(backgroundPicture.Width()),
    m_WordWidth(backgroundPicture.WordWidth()),
    m_Height(backgroundPicture.Height()),
    m_Depth(backgroundPicture.Depth())
{
  //
  // Create and initialize two BitMaps for double buffering
  //
  for(size_t i = 0; i < 2; i++)
  {
    m_pBitMapArray[i] = AllocBitMap(backgroundPicture.Width(), 
                                    backgroundPicture.Height(),
                                    backgroundPicture.Depth(),
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

    // Blit the background image into the BitMaps
    BltBitMap(backgroundPicture.GetBitMap(),
              0,
              0,
              m_pBitMapArray[i],
              0,
              0,
              backgroundPicture.Width() - 1,
              backgroundPicture.Height() - 1,
              0xC0,
              0xFF,
              NULL);
  }

  //
  // Create double buffering related objects
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

}

void GameViewBase::BlitPictureCentered(const IlbmBitmap& picture)
{

}

void GameViewBase::BlitMaskedPicture(const IlbmBitmap& picture, 
                                     ULONG left, 
                                     ULONG top)
{

}

void GameViewBase::BlitMaskedPictureCentered(const IlbmBitmap& picture)
{

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
