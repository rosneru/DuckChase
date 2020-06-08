#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <graphics/modeid.h>
#include <graphics/videocontrol.h>

#include "GameViewIntui.h"

extern struct GfxBase* GfxBase;


GameViewIntui::GameViewIntui(IlbmBitmap& backgroundPicture)
  : GameViewBase(backgroundPicture),
    m_pScreen(NULL),
    m_pDBufInfo(NULL)
{
  // Additional setting for the screen to use hires sprites
  struct TagItem vcTags[] =
  {
    {VTAG_SPRITERESN_SET, SPRITERESN_70NS},
    {TAG_DONE}
  };

  m_pScreen = OpenScreenTags(NULL,
    SA_DisplayID, PAL_MONITOR_ID|HIRES_KEY,
    SA_Depth, Depth(),
    SA_Width, Width(),
    SA_Height, Height(),
    SA_ShowTitle, FALSE,
    SA_VideoControl, vcTags,
    SA_Quiet, TRUE,
    SA_Type, CUSTOMSCREEN,
    SA_Exclusive, TRUE,
    SA_Colors32, m_BackgroundPicture.GetColors32(),
    SA_BitMap, m_pBitMapArray[0],
    TAG_DONE);

  if(m_pScreen == NULL)
  {
    throw "GameViewIntui failed to open screen.";
  }

  m_pDBufInfo = AllocDBufInfo(&(m_pScreen->ViewPort));
  if(m_pDBufInfo == NULL)
  {
    throw "GameViewIntui failed to AllocDBufInfo.";
  }

  m_pDBufInfo->dbi_SafeMessage.mn_ReplyPort = m_pSafeMessage;
  m_pDBufInfo->dbi_DispMessage.mn_ReplyPort = m_pDispMessage;

  // Now wait until rendering is allowed
  if(!m_bDBufSafeToWrite)
  {
    while (!GetMsg(m_pSafeMessage)) // As long as no dbi_SafeMessage..
    {
      Wait(1l << (m_pSafeMessage->mp_SigBit));  // ..wait for it
    }
  }

  m_bDBufSafeToWrite = true;
}


GameViewIntui::~GameViewIntui()
{
  // Autodoc: When using the synchronization features, you MUST
  // carefully insure that all messages have been replied to before
  // calling FreeDBufInfo or calling ChangeVPBitMap with the same
  // DBufInfo.
  if (!m_bDBufSafeToChange)
  {
    while (!GetMsg(m_pDispMessage)) 
    {
      Wait(1l << (m_pDispMessage->mp_SigBit));
    }
  }

  if (!m_bDBufSafeToWrite)
  {
    while (!GetMsg(m_pSafeMessage)) 
    {
      Wait(1l << (m_pSafeMessage->mp_SigBit));
    }
  }

  if(m_pDBufInfo != NULL)
  {
    FreeDBufInfo(m_pDBufInfo);
    m_pDBufInfo = NULL;
  }

  if(m_pScreen != NULL)
  {
    CloseScreen(m_pScreen);
    m_pScreen = NULL;
  }
}


struct RastPort* GameViewIntui::RastPort()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &m_pScreen->RastPort;
}


struct ViewPort* GameViewIntui::ViewPort()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &(m_pScreen->ViewPort);
}


void GameViewIntui::Render()
{
  // Render the gels
  SortGList(&(m_pScreen->RastPort));
  DrawGList(&(m_pScreen->RastPort), &(m_pScreen->ViewPort));

  // Only to be done when using VSprites
  // MrgCop(ViewAddress());
  // LoadView(ViewAddress());
  
  WaitTOF();
  
  //
  // Double buffering
  //
  if (!m_bDBufSafeToChange)
  {
    while (!GetMsg(m_pDispMessage)) 
    {
      Wait(1l << (m_pDispMessage->mp_SigBit));
    }
  }
  
  m_bDBufSafeToChange = false;

  // Be sure rendering has finished
  WaitBlit();
  
  // Switch the displayed BitMap
  ChangeVPBitMap(&m_pScreen->ViewPort, m_pBitMapArray[m_CurrentBuf], m_pDBufInfo);
  
  m_bDBufSafeToChange = false;
  m_bDBufSafeToWrite = false;
  
  // Toggle current buffer
  m_CurrentBuf ^= 1;

  // // Switch the BitMap to be used for drawing
  m_pScreen->RastPort.BitMap = m_pBitMapArray[m_CurrentBuf];

  // Now wait until rendering is allowed
  if(!m_bDBufSafeToWrite)
  {
    while (!GetMsg(m_pSafeMessage)) // As long as no dbi_SafeMessage..
    {
      Wait(1l << (m_pSafeMessage->mp_SigBit));  // ..wait for it
    }
  }

  m_bDBufSafeToWrite = true;
}

const char* GameViewIntui::ViewName() const
{
  return "Simple (Intuition) view";
}
