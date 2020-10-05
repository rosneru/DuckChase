#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <graphics/copper.h>
#include <graphics/gfxbase.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxnodes.h>
#include <graphics/videocontrol.h>
#include <hardware/custom.h>
#include <intuition/screens.h>

#include "GameViewLowLevel.h"

extern struct GfxBase* GfxBase;

// Needed for enabling display dma
extern struct Custom custom ;

GameViewLowlevel::GameViewLowlevel(OpenIlbmPictureBitMap& backgroundPicture,
                                   ULONG modeId)
  : GameViewBase(backgroundPicture),
    m_LowLevelView(modeId),
    m_LowLevelViewPort(Width(),
                       Height(),
                       Depth(),
                       modeId,
                       1L << Depth(), // TODO - Ensure min 32 colors?
                       m_ppBitMapArray[m_CurrentBuf],
                       backgroundPicture.GetColors32()),
    m_RastPort(),
    m_pOldView(NULL),
    m_pDBufInfo(NULL)
{
  // Enable display and sprite DMA in case this is the first opened view
  // since reboot
  ON_DISPLAY ;
  ON_SPRITE ;

  // Create a RastPort to draw into
  InitRastPort(&m_RastPort);
  m_RastPort.BitMap = m_LowLevelViewPort.ViewPort()->RasInfo->BitMap;

  m_LowLevelView.View()->ViewPort = m_LowLevelViewPort.ViewPort();

  // Construct preliminary Copper instruction list
  MakeVPort(m_LowLevelView.View(), m_LowLevelViewPort.ViewPort());

  // Merge preliminary lists into a real Copper list in the View
  // structure
  MrgCop(m_LowLevelView.View());

  //Save current View to restore later
  m_pOldView = GfxBase->ActiView;

  do
  {
    LoadView(NULL);
    WaitTOF();
    WaitTOF();
  }
  while(GfxBase->ActiView != NULL);

  LoadView(m_LowLevelView.View());
  WaitTOF();
  WaitTOF();

  //
  // Create double buffering related objects
  //
  m_pDBufInfo = AllocDBufInfo(m_LowLevelViewPort.ViewPort());
  if(m_pDBufInfo == NULL)
  {
    throw "GameViewLowlevel failed to AllocDBufInfo.";
  }

  m_pDBufInfo->dbi_SafeMessage.mn_ReplyPort = m_pSafePort;
  m_pDBufInfo->dbi_DispMessage.mn_ReplyPort = m_pDispPort;
}


GameViewLowlevel::~GameViewLowlevel()
{
  // Autodoc: When using the synchronization features, you MUST
  // carefully insure that all messages have been replied to before
  // calling FreeDBufInfo or calling ChangeVPBitMap with the same
  // DBufInfo.
  if (!m_IsSafeToChange)
  {
    while (!GetMsg(m_pDispPort)) 
    {
      Wait(1l << (m_pDispPort->mp_SigBit));
    }
  }

  if(m_pDBufInfo != NULL)
  {
    FreeDBufInfo(m_pDBufInfo);
    m_pDBufInfo = NULL;
  }

  if(m_LowLevelView.View() != NULL)
  {
    if (GfxBase->ActiView == m_LowLevelView.View())
    {
      do
      {
        LoadView(NULL);
        WaitTOF();
        WaitTOF();
      }
      while(GfxBase->ActiView != NULL);

      // Put back the old view
      LoadView(m_pOldView);

      // Before freeing memory wait until the old view is being rendered
      WaitTOF();
      WaitTOF();
    }
  }
}


struct RastPort* GameViewLowlevel::RastPort()
{
  return &m_RastPort;
}


struct ViewPort* GameViewLowlevel::ViewPort()
{
  return m_LowLevelViewPort.ViewPort();
}


void GameViewLowlevel::Render()
{
  // Render the gels
  SortGList(&m_RastPort);
  DrawGList(&m_RastPort, m_LowLevelViewPort.ViewPort());
  
  MrgCop(m_LowLevelView.View());
  LoadView(m_LowLevelView.View());
  WaitTOF();

  //
  // Double buffering
  //
  if (m_IsSafeToChange == false)
  {
    while (!GetMsg(m_pDispPort)) 
    {
      Wait(1l << (m_pDispPort->mp_SigBit));
    }
  }
  
  // Be sure rendering has finished
  WaitBlit();
  
  // Switch the displayed BitMap
  ChangeVPBitMap(m_LowLevelViewPort.ViewPort(),
                 m_ppBitMapArray[m_CurrentBuf], 
                 m_pDBufInfo);
  
  m_IsSafeToChange = false;
  
  // Toggle current buffer
  m_CurrentBuf ^= 1;

  // Switch the BitMap to be used for drawing
  m_RastPort.BitMap = m_ppBitMapArray[m_CurrentBuf];

  // Wait until drawing is allowed
  while (!GetMsg(m_pSafePort)) 
  {
    Wait(1l << (m_pSafePort->mp_SigBit));
  }
}


const char* GameViewLowlevel::ViewName() const
{
  return "Lowlevel view";
}
