#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <graphics/modeid.h>
#include <graphics/videocontrol.h>
#include <intuition/pointerclass.h>

#include "GameViewIntui.h"

extern struct GfxBase* GfxBase;


GameViewIntui::GameViewIntui(OpenIlbmPictureBitMap& backgroundPicture)
  : GameViewBase(backgroundPicture),
    m_pScreen(NULL),
    m_pEmptyBitmap(NULL),
    m_pEmptyPointer(NULL),
    m_pWindow(NULL),
    m_pDBufInfo(NULL)
{
  // Additional setting for the screen
  struct TagItem vcTags[] =
  {
    {VTAG_SPRITERESN_SET, SPRITERESN_70NS}, // use HiRes sprites
    {VC_IntermediateCLUpdate, FALSE},       // speeds up LoadRGB(), SetRGB(), ScrollVPort(), and ChangeVPBitMap()
    {TAG_DONE}
  };

  m_pScreen = OpenScreenTags(NULL,
    SA_DisplayID, PAL_MONITOR_ID|HIRES_KEY,
    SA_Depth, Depth(),
    SA_Width, Width(),
    SA_Height, Height(),
    SA_ShowTitle, FALSE,
    SA_VideoControl, vcTags,
    SA_Interleaved, TRUE,
    SA_Quiet, TRUE,
    SA_Type, CUSTOMSCREEN,
    SA_Exclusive, TRUE,
    SA_Colors32, backgroundPicture.GetColors32(),
    TAG_DONE);

  if(m_pScreen == NULL)
  {
    throw "GameViewIntui failed to open screen.";
  }

  m_pEmptyBitmap = AllocBitMap(16, 16, 2, BMF_CLEAR, NULL);
  if(m_pEmptyBitmap == NULL)
  {
    throw "GameViewIntui failed to allocate empty pointer BitMap.";
  }

  m_pEmptyPointer = NewObject( NULL, "pointerclass",
    POINTERA_BitMap, m_pEmptyBitmap,
    POINTERA_XOffset, -6,
    POINTERA_WordWidth, 1,
    POINTERA_XResolution, POINTERXRESN_HIRES,
    POINTERA_YResolution, POINTERYRESN_HIGH,
    TAG_DONE );

  if(m_pEmptyPointer == NULL)
  {
    throw "GameViewIntui failed to allocate empty pointer object.";
  }

  m_pWindow = OpenWindowTags(NULL,
    WA_Activate, TRUE,
    WA_Borderless, TRUE,
    WA_Backdrop, TRUE,
    WA_CustomScreen, m_pScreen,
    WA_Pointer, m_pEmptyPointer,
    TAG_DONE);

  if(m_pWindow == NULL)
  {
    throw "GameViewIntui failed to open window.";
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

  if(m_pWindow != NULL)
  {
    CloseWindow(m_pWindow);
    m_pWindow = NULL;
  }

  if(m_pEmptyPointer != NULL)
  {
    DisposeObject(m_pEmptyPointer);
    m_pEmptyPointer = NULL;
  }

  if(m_pEmptyBitmap != NULL)
  {
    FreeBitMap(m_pEmptyBitmap);
    m_pEmptyBitmap = NULL;
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
  // Render the GELS if there are some
  if(m_pScreen->RastPort.GelsInfo != NULL)
  {
    SortGList(&(m_pScreen->RastPort));
    DrawGList(&(m_pScreen->RastPort), &(m_pScreen->ViewPort));
  }

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
