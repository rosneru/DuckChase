#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <graphics/copper.h>
#include <graphics/gfxbase.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxnodes.h>
#include <graphics/videocontrol.h>

#include "GameViewGfxLib.h"

extern struct GfxBase* GfxBase;


GameViewGfxLib::GameViewGfxLib(short viewWidth,
                               short viewHeight,
                               short viewDepth)
  : m_ViewWidth(viewWidth),
    m_ViewHeight(viewHeight),
    m_ViewDepth(viewDepth),
    m_pOldView(NULL),
    m_View(),
    m_pViewPort(NULL),
    m_RastPort(),
    m_DimensionInfo(),
    m_pColorMap(NULL),
    m_pViewExtra(NULL),
    m_pMonitorSpec(NULL),
    m_pViewPortExtra(NULL),
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


GameViewGfxLib::~GameViewGfxLib()
{

}

bool GameViewGfxLib::Open()
{
  if(m_pViewExtra != NULL)
  {
    m_InitError = IE_AlreadyInitialized;
    Close();
    return false;
  }

  m_InitError = IE_None;

  struct RasInfo rasInfo;
  ULONG modeID;

  // Initialize the View and set View.Modes
  InitView(&m_View);

  // Form the ModeID from values in <displayinfo.h>
  modeID = DEFAULT_MONITOR_ID | HIRES_KEY;

  //  Make the ViewExtra structure
  m_pViewExtra = (struct ViewExtra*) GfxNew(VIEW_EXTRA_TYPE);
  if (m_pViewExtra == NULL)
  {
    m_InitError = IE_GettingViewExtra;
    Close();
    return false;
  }

  // Attach the ViewExtra to the View
  GfxAssociate(&m_View, m_pViewExtra);
  m_View.Modes |= EXTEND_VSTRUCT;

  // Create and attach a MonitorSpec to the ViewExtra
  m_pMonitorSpec = OpenMonitor(NULL, modeID);
  if (m_pMonitorSpec == NULL)
  {
    m_InitError = IE_GettingMonSpec;
    Close();
    return false;
  }

  m_pViewExtra->Monitor = m_pMonitorSpec;

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

  // Create a RastPort to draw into
  InitRastPort(&m_RastPort);
  m_RastPort.BitMap = m_pBitMapArray[0];
  SetRast(&m_RastPort, 0);

  // Initialize the RasInfo
  rasInfo.BitMap = m_pBitMapArray[0];
  rasInfo.RxOffset = 0;
  rasInfo.RyOffset = 0;
  rasInfo.Next = NULL;

  // Initialize the ViewPort
  m_pViewPort = (struct ViewPort*) AllocVec(sizeof(struct ViewPort),
                                            MEMF_CLEAR);
  if (m_pViewPort == NULL)
  {
    m_InitError = IE_GettingViewPort;
    Close();
    return false;
  }

  InitVPort(m_pViewPort);
  m_View.ViewPort = m_pViewPort; // Link the ViewPort into the View
  m_pViewPort->RasInfo = &rasInfo;
  m_pViewPort->DWidth = m_ViewWidth;
  m_pViewPort->DHeight = m_ViewHeight;

  // Make a ViewPortExtra and get ready to attach it
  m_pViewPortExtra = (struct ViewPortExtra*) GfxNew(VIEWPORT_EXTRA_TYPE);

  if (m_pViewPortExtra == NULL)
  {
    m_InitError = IE_GettingVPExtra;
    Close();
    return false;
  }

  // Creating a  tag item array for VideoControl and fill the values
  struct TagItem videoCtrlTags[] =
  {
    {VTAG_ATTACH_CM_SET, NULL },
    {VTAG_VIEWPORTEXTRA_SET, NULL },
    {VTAG_NORMAL_DISP_SET, NULL },
    {VTAG_END_CM, NULL }
  };

  videoCtrlTags[1].ti_Data = (ULONG)m_pViewPortExtra;

  // Initialize the DisplayClip field of the ViewPortExtra
  if (GetDisplayInfoData(NULL, (UBYTE *)&m_DimensionInfo,
    sizeof(m_DimensionInfo), DTAG_DIMS, modeID) == 0)
  {
    m_InitError = IE_GettingDimInfo;
    Close();
    return false;
  }

  m_pViewPortExtra->DisplayClip = m_DimensionInfo.Nominal;

  // Make a DisplayInfo and get ready to attach it
  if (!(videoCtrlTags[2].ti_Data = (ULONG) FindDisplayInfo(modeID)))
  {
    m_InitError = IE_GettingDisplayInfo;
    Close();
    return false;
  }

  // Initialize the ColorMap, 3 planes deep, so 8 entries
  m_pColorMap = GetColorMap(m_ViewNumColors);
  if (m_pColorMap == NULL)
  {
    m_InitError = IE_GettingCM;
    Close();
    return false;
  }

  // Get ready to attach the ColorMap, Release 2-style
  videoCtrlTags[0].ti_Data = (ULONG)m_pViewPort;

  // Attach the color map and Release 2 extended structures
  if (VideoControl(m_pColorMap, videoCtrlTags) != NULL)
  {
    m_InitError = IE_AttachExtStructs;
    Close();
    return false;
  }

  // Construct preliminary Copper instruction list
  MakeVPort(&m_View, m_pViewPort);

  // Merge preliminary lists into a real Copper list in the View
  // structure
  MrgCop(&m_View);

  // Save current View to restore later
  m_pOldView = GfxBase->ActiView;

  LoadView(&m_View);
  WaitTOF();
  WaitTOF();

  return true;
}

void GameViewGfxLib::Close()
{
  if(GfxBase->ActiView == &m_View)
  {
    // Put back the old view
    LoadView(m_pOldView);

    // Before freeing memory wait until the old view is being rendered
    WaitTOF();
    WaitTOF();
  }

  // Free the color map created by GetColorMap()
  if (m_pColorMap != NULL)
  {
    FreeColorMap(m_pColorMap);
    m_pColorMap = NULL;
  }

  if(m_View.LOFCprList != NULL)
  {
    // Deallocate the hardware Copper list created by MrgCop()
    FreeCprList(m_View.LOFCprList);
    m_View.LOFCprList = NULL;
  }

  if(m_View.SHFCprList != NULL)
  {
    // Deallocate also the interlace-only hardware Copper list
    FreeCprList(m_View.SHFCprList);
    m_View.SHFCprList = NULL;
  }

  // Free the ViewPortExtra created by GfxNew()
  if (m_pViewPortExtra != NULL)
  {
    GfxFree(m_pViewPortExtra);
    m_pViewPortExtra = NULL;
  }

  // Free the ViewPort and it copper lists
  if (m_pViewPort != NULL)
  {
    // Free all intermediate Copper lists created by MakeVPort()
    FreeVPortCopLists(m_pViewPort);

    // And now the ViewPort itself
    FreeVec(m_pViewPort);
    m_pViewPort = NULL;
  }

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

  // Free the MonitorSpec created with OpenMonitor()
  if (m_pMonitorSpec != NULL)
  {
    CloseMonitor(m_pMonitorSpec);
    m_pMonitorSpec = NULL;
  }

  // Free the ViewExtra created with GfxNew()
  if (m_pViewExtra != NULL)
  {
    GfxFree(m_pViewExtra);
    m_pViewExtra = NULL;
  }
}


short GameViewGfxLib::Width()
{
  return m_ViewWidth;
}


short GameViewGfxLib::Height()
{
  return m_ViewHeight;
}


short GameViewGfxLib::Depth()
{
  return m_ViewDepth;
}


struct RastPort* GameViewGfxLib::RastPort()
{
  return &m_RastPort;
}


struct ViewPort* GameViewGfxLib::ViewPort()
{
  return m_pViewPort;
}


void GameViewGfxLib::Render()
{
  if(GfxBase->ActiView != &m_View)
  {
    // Not initialized
    return;
  }

  SortGList(&m_RastPort);
  DrawGList(&m_RastPort, m_pViewPort);

  // Double buffering: One BitMap is for the view
  m_pViewPort->RasInfo->BitMap = m_pBitMapArray[m_FrameToggle];

  WaitTOF();

  MrgCop(&m_View);
  LoadView(&m_View);

  // Double buffering: Toggle the BitMap pointer
  m_FrameToggle ^= 1;

  // Double buffering: Now the other BitMap is for drawing
  m_RastPort.BitMap = m_pBitMapArray[m_FrameToggle];
}

const char* GameViewGfxLib::LastError() const
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

const char* GameViewGfxLib::ViewName() const
{
  return "Advanced (Gfx) view";
}
