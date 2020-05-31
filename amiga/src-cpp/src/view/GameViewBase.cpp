#include <stdlib.h>

#include "clib/exec_protos.h"
#include "clib/graphics_protos.h"
#include "GameViewBase.h"

GameViewBase::GameViewBase(IlbmBitMap& backgroundPicture)
  : m_BackgroundPicture(backgroundPicture),
    m_pBitMapArray(),
    m_BorderTop(0),
    m_BorderLeft(0),
    m_BorderBottom(backgroundPicture.Height() - 1),
    m_BorderRight(backgroundPicture.Width() - 1),
    m_IsDoubleBuffered(true),
    m_bDBufSafeToChange(true),
    m_CurrentBuf(0),
    m_pDBufMsgReadyToWriteOldBM(NULL),
    m_pDBufMsgNewBitMapDisplayed(NULL)
{
  //
  // Create and initialize two BitMaps for double buffering
  //
  for(size_t i = 0; i < 2; i++)
  {
    m_pBitMapArray[i] = AllocBitMap(m_BackgroundPicture.Width(), 
                                    m_BackgroundPicture.Height(),
                                    m_BackgroundPicture.Depth(),
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
    BltBitMap(m_BackgroundPicture.GetBitMap(),
              0,
              0,
              m_pBitMapArray[i],
              0,
              0,
              m_BackgroundPicture.Width(),
              m_BackgroundPicture.Height(),
              0xC0,
              0xFF,
              NULL);
  }

  //
  // Create double buffering related objects
  //
  m_pDBufMsgReadyToWriteOldBM = CreateMsgPort();
  m_pDBufMsgNewBitMapDisplayed = CreateMsgPort();

  if(m_pDBufMsgReadyToWriteOldBM == NULL || m_pDBufMsgNewBitMapDisplayed == NULL )
  {
    throw "GameViewBase failed to create MsgPort.";
  }
}


GameViewBase::~GameViewBase()
{
  if(m_pDBufMsgReadyToWriteOldBM != NULL)
  {
    DeleteMsgPort(m_pDBufMsgReadyToWriteOldBM);
  }

  if(m_pDBufMsgNewBitMapDisplayed != NULL)
  {
    DeleteMsgPort(m_pDBufMsgNewBitMapDisplayed);
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

IlbmBitMap& GameViewBase::BackgroundPicture()
{
  return m_BackgroundPicture;
}

long GameViewBase::Width()
{
  return m_BackgroundPicture.Width();
}

long GameViewBase::WordWidth()
{
  return m_BackgroundPicture.WordWidth();
}

long GameViewBase::Height()
{
  return m_BackgroundPicture.Height();
}


long GameViewBase::Depth()
{
  return m_BackgroundPicture.Depth();
}


long GameViewBase::BorderTop()
{
  return m_BorderTop;
}

long GameViewBase::BorderLeft()
{
  return m_BorderLeft;
}

long GameViewBase::BorderBottom()
{
  return m_BorderBottom;
}

long GameViewBase::BorderRight()
{
  return m_BorderRight;
}

void GameViewBase::DisableDoubleBuf()
{
  m_IsDoubleBuffered = false;
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
