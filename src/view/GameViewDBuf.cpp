#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <graphics/gfxbase.h>
#include <graphics/modeid.h>
#include <graphics/videocontrol.h>
#include <intuition/pointerclass.h>

#include "GameViewDBuf.h"

GameViewDBuf::GameViewDBuf(OpenIlbmPictureBitMap& backgroundPicture)
  : GameViewBase(backgroundPicture, false),
    m_pScreen(NULL),
    m_pEmptyBitmap(NULL),
    m_pEmptyPointer(NULL),
    m_pWindow(NULL),
    m_ppScreenBuf()  // all pointers in array initialized to NULL
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
    throw "GameViewDBuf failed to open screen.";
  }

  m_pEmptyBitmap = AllocBitMap(16, 16, 2, BMF_CLEAR, NULL);
  if(m_pEmptyBitmap == NULL)
  {
    throw "GameViewDBuf failed to allocate empty pointer BitMap.";
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
    throw "GameViewDBuf failed to allocate empty pointer object.";
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
    throw "GameViewDBuf failed to open window.";
  }

  m_ppScreenBuf[0] = AllocScreenBuffer(m_pScreen, NULL, SB_SCREEN_BITMAP);
  m_ppScreenBuf[1] = AllocScreenBuffer(m_pScreen, NULL, 0);

  for (ULONG i = 0; i < 2; i++)
  {
    m_ppScreenBuf[i]->sb_DBufInfo->dbi_DispMessage.mn_ReplyPort = m_pDispPort;
    m_ppScreenBuf[i]->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = m_pSafePort;
  }

  // Display the background picture
  BlitPicture(backgroundPicture);
}

GameViewDBuf::~GameViewDBuf()
{
  cleanup();
}


void GameViewDBuf::cleanup()
{
  // Autodoc: When using the synchronization features, you MUST
  // carefully insure that all messages have been replied to before
  // calling FreeDBufInfo or calling ChangeVPBitMap with the same
  // DBufInfo.
  if ((m_pDispPort != NULL) && !m_IsSafeToChange)
  {
    while (!GetMsg(m_pDispPort)) 
    {
      Wait(1l << (m_pDispPort->mp_SigBit));
    }
  }

  if ((m_pSafePort != NULL) && !m_IsSafeToWrite)
  {
    while (!GetMsg(m_pSafePort)) 
    {
      Wait(1l << (m_pSafePort->mp_SigBit));
    }
  }

  for(ULONG i = 0; i < 2; i++)
  {
    if(m_ppScreenBuf[i] != NULL)
    {
      WaitBlit();
      FreeScreenBuffer(m_pScreen, m_ppScreenBuf[i]);
      m_ppScreenBuf[i] = NULL;
    }
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


struct RastPort* GameViewDBuf::RastPort()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &m_pScreen->RastPort;
}


struct ViewPort* GameViewDBuf::ViewPort()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &(m_pScreen->ViewPort);
}


void GameViewDBuf::Render()
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
  if (!m_IsSafeToChange)
  {
    while (!GetMsg(m_pDispPort)) 
    {
      Wait(1l << (m_pDispPort->mp_SigBit));
    }
  }
  
  m_IsSafeToChange = false;

  // Be sure rendering has finished
  WaitBlit();
  
  // Flip the ScreenBuffer
  if (ChangeScreenBuffer(m_pScreen, m_ppScreenBuf[m_CurrentBuf]))
  {
    // Flip.
    m_CurrentBuf = m_CurrentBuf ^ 1;
  }

  m_IsSafeToChange = false;
  m_IsSafeToWrite = false;

  // // // Switch the BitMap to be used for drawing
  // m_pScreen->RastPort.BitMap = m_ppBitMapArray[m_CurrentBuf];

  // Now wait until rendering is allowed
  if(!m_IsSafeToWrite)
  {
    while (!GetMsg(m_pSafePort)) // As long as no dbi_SafeMessage..
    {
      Wait(1l << (m_pSafePort->mp_SigBit));  // ..wait for it
    }
  }

  m_IsSafeToWrite = true;
}

const char* GameViewDBuf::ViewName() const
{
  return "Simple (Intuition) view";
}
