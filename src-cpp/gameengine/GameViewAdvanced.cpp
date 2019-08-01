#include <clib/graphics_protos.h>
#include <graphics/copper.h>
#include <graphics/gfxbase.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxnodes.h>
#include <graphics/videocontrol.h>

#include "GameViewAdvanced.h"

extern struct GfxBase* GfxBase;


GameViewAdvanced::GameViewAdvanced(short viewWidth,
                                   short viewHeight,
                                   short viewDepth)
  : m_ViewWidth(viewWidth),
    m_ViewHeight(viewHeight),
    m_ViewDepth(viewDepth),
    m_pOldView(NULL),
    m_View(),
    m_ViewPort(),
    m_BitMap1(),
    m_BitMap2(),
    m_RastPort(),
    m_DimensionInfo(),
    m_pColorMap(NULL),
    m_pViewExtra(NULL),
    m_pMonitorSpec(NULL),
    m_pViewPortExtra(NULL),
    m_InitError(IE_None),
    m_BufToggle(false)
{
  m_ViewNumColors = 1;
  for(int i = 0; i < viewDepth; i++)
  {
    m_ViewNumColors *= 2;
  }
}


GameViewAdvanced::~GameViewAdvanced()
{

}

bool GameViewAdvanced::Open()
{
  if(m_pViewExtra != NULL)
  {
    m_InitError = IE_AlreadyInitialized;
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
    return false;
  }

  m_pViewExtra->Monitor = m_pMonitorSpec;

  // Initialize the BitMaps
  InitBitMap(&m_BitMap1, m_ViewDepth, m_ViewWidth, m_ViewHeight);
  InitBitMap(&m_BitMap2, m_ViewDepth, m_ViewWidth, m_ViewHeight);

  // Set the plane pointers to NULL so the cleanup routine
  // will know if they were used
  for (int depth = 0; depth < m_ViewDepth; depth++)
  {
    m_BitMap1.Planes[depth] = NULL;
    m_BitMap2.Planes[depth] = NULL;
  }

  // Allocate space for BitMap
  for (int depth = 0; depth < m_ViewDepth; depth++)
  {
    m_BitMap1.Planes[depth] = (PLANEPTR)
      AllocRaster(m_ViewWidth, m_ViewHeight);

    if (m_BitMap1.Planes[depth] == NULL)
    {
      m_InitError = IE_GettingBitPlanes;
      return false;
    }

    m_BitMap2.Planes[depth] = (PLANEPTR)
      AllocRaster(m_ViewWidth, m_ViewHeight);

    if (m_BitMap2.Planes[depth] == NULL)
    {
      m_InitError = IE_GettingBitPlanes;
      return false;
    }
  }

  // Create a RastPort to draw into
  InitRastPort(&m_RastPort);
  m_RastPort.BitMap = &m_BitMap1;
  SetRast(&m_RastPort, 0);

  // Initialize the RasInfo
  rasInfo.BitMap = &m_BitMap1;
  rasInfo.RxOffset = 0;
  rasInfo.RyOffset = 0;
  rasInfo.Next = NULL;

  // Initialize the ViewPort
  InitVPort(&m_ViewPort);
  m_View.ViewPort = &m_ViewPort; // Link the ViewPort into the View
  m_ViewPort.RasInfo = &rasInfo;
  m_ViewPort.DWidth = m_ViewWidth;
  m_ViewPort.DHeight = m_ViewHeight;

  // Make a ViewPortExtra and get ready to attach it
  m_pViewPortExtra = (struct ViewPortExtra*) GfxNew(VIEWPORT_EXTRA_TYPE);

  if (m_pViewPortExtra == NULL)
  {
    m_InitError = IE_GettingVPExtra;
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
    return false;
  }

  m_pViewPortExtra->DisplayClip = m_DimensionInfo.Nominal;

  // Make a DisplayInfo and get ready to attach it
  if (!(videoCtrlTags[2].ti_Data = (ULONG) FindDisplayInfo(modeID)))
  {
    m_InitError = IE_GettingDisplayInfo;
    return false;
  }

  // Initialize the ColorMap, 3 planes deep, so 8 entries
  m_pColorMap = GetColorMap(m_ViewNumColors);
  if (m_pColorMap == NULL)
  {
    m_InitError = IE_GettingCM;
    return false;
  }

  /* Get ready to attach the ColorMap, Release 2-style */
  videoCtrlTags[0].ti_Data = (ULONG)&m_ViewPort;

  /* Attach the color map and Release 2 extended structures */
  if (VideoControl(m_pColorMap, videoCtrlTags))
  {
    m_InitError = IE_AttachExtStructs;
    return false;
  }

  // Construct preliminary Copper instruction list
  MakeVPort(&m_View, &m_ViewPort);


  // Clear the ViewPort
  for (int depth = 0; depth < m_ViewDepth; depth++)
  {
    UBYTE* pPlane = (UBYTE*) m_BitMap1.Planes[depth];
    BltClear(pPlane, (m_BitMap1.BytesPerRow * m_BitMap1.Rows), 1L);
  }

  // Merge preliminary lists into a real Copper list in the View
  // structure
  MrgCop(&m_View);

  // Save current View to restore later
  m_pOldView = GfxBase->ActiView;

  LoadView(NULL); // TODO try without this
  WaitTOF();
  WaitTOF();

  LoadView(&m_View);

  return true;
}

void GameViewAdvanced::Close()
{
  if(GfxBase->ActiView == &m_View)
  {
    // Put back the old view
    LoadView(m_pOldView);

    // Before freeing memory wait until the old view is being rendered
    WaitTOF();
    WaitTOF();
  }

  //  Free the color map created by GetColorMap()
  if (m_pColorMap != NULL)
  {
    FreeColorMap(m_pColorMap);
  }

  // Free all intermediate Copper lists from created by MakeVPort()
  FreeVPortCopLists(&m_ViewPort);

  if(m_View.LOFCprList != NULL)
  {
    // Deallocate the hardware Copper list created by MrgCop()
    FreeCprList(m_View.LOFCprList);
  }

  if(m_View.SHFCprList != NULL)
  {
    // Deallocate also the interlace-only hardware Copper list
    FreeCprList(m_View.SHFCprList);
  }

  // Free the ViewPortExtra created by GfxNew()
  if (m_pViewPortExtra != NULL)
  {
    GfxFree(m_pViewPortExtra);
  }

  //  Free the BitPlanes drawing area
  for (int depth = 0; depth < m_ViewDepth; depth++)
  {
    if (m_BitMap1.Planes[depth] != NULL)
    {
      FreeRaster(m_BitMap1.Planes[depth], m_ViewWidth, m_ViewHeight);
    }

    if (m_BitMap2.Planes[depth] != NULL)
    {
      FreeRaster(m_BitMap2.Planes[depth], m_ViewWidth, m_ViewHeight);
    }

  }

  // Free the MonitorSpec created with OpenMonitor()
  if (m_pMonitorSpec != NULL)
  {
    CloseMonitor(m_pMonitorSpec);
  }

  // Free the ViewExtra created with GfxNew()
  if (m_pViewExtra != NULL)
  {
    GfxFree(m_pViewExtra);
  }
}

struct RastPort* GameViewAdvanced::RastPort()
{
  return &m_RastPort;
}

struct ViewPort* GameViewAdvanced::ViewPort()
{
  return &m_ViewPort;
}

struct View* GameViewAdvanced::View()
{
  return &m_View;
}

void GameViewAdvanced::Render()
{
  SortGList(&m_RastPort);
  DrawGList(&m_RastPort, &m_ViewPort);
  WaitTOF();

  MrgCop(&m_View);
  LoadView(&m_View);

  // Switch the buffers
  if(m_BufToggle == false)
  {
    m_ViewPort.RasInfo->BitMap = &m_BitMap2;
    m_RastPort.BitMap = &m_BitMap2;
    m_BufToggle = true;
  }
  else
  {
    m_ViewPort.RasInfo->BitMap = &m_BitMap1;
    m_RastPort.BitMap = &m_BitMap1;
    m_BufToggle = false;
  }
}

const char* GameViewAdvanced::LastError() const
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

    case IE_GettingBitPlanes:
      return "Could not get BitPlanes.\n";
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
