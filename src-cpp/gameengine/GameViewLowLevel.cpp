#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <graphics/copper.h>
#include <graphics/gfxbase.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxnodes.h>
#include <graphics/videocontrol.h>

#include "GameViewLowLevel.h"

extern struct GfxBase* GfxBase;


GameViewLowlevel::GameViewLowlevel(short viewWidth,
                               short viewHeight,
                               short viewDepth)
  : m_ViewWidth(viewWidth),
    m_ViewHeight(viewHeight),
    m_ViewDepth(viewDepth),
    m_pOldView(NULL),
    m_pView(NULL),
    m_pViewPort(NULL),
    m_RastPort(),
    m_FrameToggle(0),
    m_pBitMapArray(),
    m_InitError(IE_None)
{
  m_ViewNumColors = 1;
  for(int i = 0; i < viewDepth; i++)
  {
    m_ViewNumColors *= 2;
  }
}


GameViewLowlevel::~GameViewLowlevel()
{

}

bool GameViewLowlevel::Open()
{
  if(m_pViewPort != NULL)
  {
    m_InitError = IE_AlreadyInitialized;
    Close();
    return false;
  }

  m_InitError = IE_None;

  // Initialize the BitMaps
  for(int i = 0; i < 2; i++)
  {
    // Allocate memory for BitMap i
    m_pBitMapArray[i] = (struct BitMap *)
     AllocVec(sizeof(struct BitMap), MEMF_CHIP);

    if(m_pBitMapArray[i] == NULL)
    {
      m_InitError = IE_GettingBitMapMem;
      Close();
      return false;
    }

    // Init BitMap i
    InitBitMap(m_pBitMapArray[i], m_ViewDepth, m_ViewWidth,
               m_ViewHeight);

    // Set the plane pointers to NULL so the cleanup routine will know
    // if they were used
    for (int depth = 0; depth < m_ViewDepth; depth++)
    {
      m_pBitMapArray[i]->Planes[depth] = NULL;
    }

    // Allocate memory for the BitPlanes
    for (int depth = 0; depth < m_ViewDepth; depth++)
    {
      m_pBitMapArray[i]->Planes[depth] = (PLANEPTR)
        AllocRaster(m_ViewWidth, m_ViewHeight);

      if (m_pBitMapArray[i]->Planes[depth] == NULL)
      {
        m_InitError = IE_GettingBitPlanes;
        return false;
        Close();
      }

      // Set all bits of this newly created BitPlane to 0
      BltClear(m_pBitMapArray[i]->Planes[depth],
               (m_ViewWidth / 8) * m_ViewHeight, 1);
    }
  }

  ULONG modeId = (PAL_MONITOR_ID | HIRES_KEY);

  if(m_LowLevelView.Create(modeId) == false)
  {
    Close();
    return false;
  }

  m_pView = m_LowLevelView.View();

  if(m_LowLevelViewPort.Create(m_ViewWidth, 
                               m_ViewHeight, 
                               m_ViewDepth,  
                               modeId, 
                               m_ViewNumColors, 
                               m_pBitMapArray[0]) == false)
  {
    Close();
    return false;
  }

  m_pViewPort = m_LowLevelViewPort.ViewPort();


  // Merge preliminary lists into a real Copper list in the View
  // structure
  MrgCop(m_pView);

  // Save current View to restore later
  m_pOldView = GfxBase->ActiView;

  LoadView(m_pView);
  WaitTOF();
  WaitTOF();

  return true;
}

void GameViewLowlevel::Close()
{
  if(GfxBase->ActiView == m_pView)
  {
    // Put back the old view
    LoadView(m_pOldView);

    // Before freeing memory wait until the old view is being rendered
    WaitTOF();
    WaitTOF();
  }

  m_LowLevelViewPort.Delete();
  m_LowLevelView.Delete();


  //  Free the double buffers
  for(int i = 0; i < 2; i++)
  {
    if(m_pBitMapArray[i] != NULL)
    {
      // Free all BitPlanes of this buffer
      for (int depth = 0; depth < m_ViewDepth; depth++)
      {
        if (m_pBitMapArray[i]->Planes[depth] != NULL)
        {
          FreeRaster(m_pBitMapArray[i]->Planes[depth],
                     m_ViewWidth, m_ViewHeight);

          m_pBitMapArray[i]->Planes[depth] = NULL;
        }
      }

      // Then free the buffer itself
      FreeVec(m_pBitMapArray[i]);
      m_pBitMapArray[i] = NULL;
    }
  }
}


short GameViewLowlevel::Width()
{
  return m_ViewWidth;
}


short GameViewLowlevel::Height()
{
  return m_ViewHeight;
}


short GameViewLowlevel::Depth()
{
  return m_ViewDepth;
}


struct RastPort* GameViewLowlevel::RastPort()
{
  return &m_RastPort;
}


struct ViewPort* GameViewLowlevel::ViewPort()
{
  return m_pViewPort;
}


void GameViewLowlevel::Render()
{
  if(GfxBase->ActiView != m_pView)
  {
    // Not initialized
    return;
  }

  SortGList(&m_RastPort);
  DrawGList(&m_RastPort, m_pViewPort);

  // Double buffering: One BitMap is for the view
  m_pViewPort->RasInfo->BitMap = m_pBitMapArray[m_FrameToggle];

  WaitTOF();

  MrgCop(m_pView);
  LoadView(m_pView);

  // Double buffering: Toggle the BitMap pointer
  m_FrameToggle ^= 1;

  // Double buffering: Now the other BitMap is for drawing
  m_RastPort.BitMap = m_pBitMapArray[m_FrameToggle];
}

const char* GameViewLowlevel::LastError() const
{
  switch(m_InitError)
  {
    case IE_None:
      return "No error: init done successfully.";
      break;

    case IE_AlreadyInitialized:
      return "GameView already initialized.";
      break;

    case IE_GettingViewExtra:
      return "Could not get ViewExtra.\n";
      break;

    case IE_GettingMonSpec:
      return "Could not get MonitorSpec.\n";
      break;

    case IE_GettingBitMapMem:
      return "Could not get BitMap memory.\n";
      break;

    case IE_GettingBitPlanes:
      return "Could not get BitPlanes.\n";
      break;

    case IE_GettingViewPort:
      return "Could not get ViewPort memory.\n";
      break;

    case IE_GettingVPExtra:
      return "Could not get ViewPortExtra.\n";
      break;

    case IE_GettingDimInfo:
      return "Could not get DimensionInfo.\n";
      break;

    case IE_GettingDisplayInfo:
      return "Could not get DisplayInfo.\n";
      break;

    case IE_GettingCM:
      return "Could not get ColorMap.\n";
      break;

    case IE_AttachExtStructs:
      return "Could not attach extended structures.\n";
      break;

    default:
      return "Unknown state";
      break;
  }
}

const char* GameViewLowlevel::ViewName() const
{
  return "Lowlevel view";
}
