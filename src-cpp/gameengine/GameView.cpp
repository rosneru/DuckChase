#include <clib/graphics_protos.h>
#include <graphics/gfxbase.h>

#include "GameView.h"

extern struct GfxBase* GfxBase;


GameView::GameView(short viewWidth, short viewHeight, short viewDepth)
  : m_ViewWidth(viewWidth),
    m_ViewHeight(viewHeight),
    m_ViewDepth(viewDepth),
    oldview(NULL),
    //viewPort(0),
    //bitMap1(0),
    //bitMap2(0),
    cm(NULL),
    vextra(NULL),
    monspec(NULL),
    vpextra(NULL),
    m_BufToggle(false)
    //dimquery(0)
{
  m_ViewNumColors = 1;
  for(int i = 0; i < viewDepth; i++)
  {
    m_ViewNumColors *= 2;
  }
}


GameView::~GameView()
{

}

bool GameView::Init()
{
  m_InitError = IE_None;

  struct RasInfo rasInfo;
  ULONG modeID;

  struct TagItem vcTags[] =
  {
    {VTAG_ATTACH_CM_SET, NULL },
    {VTAG_VIEWPORTEXTRA_SET, NULL },
    {VTAG_NORMAL_DISP_SET, NULL },
    {VTAG_END_CM, NULL }
  };


  // Save current View to restore later
  oldview = GfxBase->ActiView;

  LoadView(NULL);
  WaitTOF();
  WaitTOF();

  // Initialize the View and set View.Modes
  InitView(&view);

  // Form the ModeID from values in <displayinfo.h>
  modeID = DEFAULT_MONITOR_ID | HIRES_KEY;

  //  Make the ViewExtra structure
  vextra = (struct ViewExtra*) GfxNew(VIEW_EXTRA_TYPE);
  if (vextra == NULL)
  {
    m_InitError = IE_GettingViewExtra;
    return false;
  }

  // Attach the ViewExtra to the View
  GfxAssociate(&view, vextra);
  view.Modes |= EXTEND_VSTRUCT;

  // Create and attach a MonitorSpec to the ViewExtra
  monspec = OpenMonitor(NULL, modeID);
  if (monspec == NULL)
  {
    m_InitError = IE_GettingMonSpec;
    return false;
  }

  vextra->Monitor = monspec;

  // Initialize the BitMaps
  InitBitMap(&bitMap1, m_ViewDepth, m_ViewWidth, m_ViewHeight);
  InitBitMap(&bitMap2, m_ViewDepth, m_ViewWidth, m_ViewHeight);

  // Set the plane pointers to NULL so the cleanup routine
  // will know if they were used
  for (int depth = 0; depth < m_ViewDepth; depth++)
  {
    bitMap1.Planes[depth] = NULL;
    bitMap2.Planes[depth] = NULL;
  }

  // Allocate space for BitMap
  for (int depth = 0; depth < m_ViewDepth; depth++)
  {
    bitMap1.Planes[depth] = (PLANEPTR) 
      AllocRaster(m_ViewWidth, m_ViewHeight);

    if (bitMap1.Planes[depth] == NULL)
    {
      m_InitError = IE_GettingBitPlanes;
      return false;
    }

    bitMap2.Planes[depth] = (PLANEPTR) 
      AllocRaster(m_ViewWidth, m_ViewHeight);

    if (bitMap2.Planes[depth] == NULL)
    {
      m_InitError = IE_GettingBitPlanes;
      return false;
    }
  }

  // Create a RastPort to draw into
  InitRastPort(&rastPort);
  rastPort.BitMap = &bitMap1;
  SetRast(&rastPort, 0);

  // Initialize the RasInfo
  rasInfo.BitMap = &bitMap1;
  rasInfo.RxOffset = 0;
  rasInfo.RyOffset = 0;
  rasInfo.Next = NULL;

  // Initialize the ViewPort
  InitVPort(&viewPort);
  view.ViewPort = &viewPort; // Link the ViewPort into the View
  viewPort.RasInfo = &rasInfo;
  viewPort.DWidth = m_ViewWidth;
  viewPort.DHeight = m_ViewHeight;

  // Make a ViewPortExtra and get ready to attach it
  vpextra = (struct ViewPortExtra*) GfxNew(VIEWPORT_EXTRA_TYPE);

  if (vpextra == NULL)
  {
    m_InitError = IE_GettingVPExtra;
    return false;
  }

  vcTags[1].ti_Data = (ULONG)vpextra;

  // Initialize the DisplayClip field of the ViewPortExtra
  if (GetDisplayInfoData(NULL, (UBYTE *)&dimquery,
    sizeof(dimquery), DTAG_DIMS, modeID) == 0)
  {
    m_InitError = IE_GettingDimInfo;
    return false;
  }

  vpextra->DisplayClip = dimquery.Nominal;

  // Make a DisplayInfo and get ready to attach it
  if (!(vcTags[2].ti_Data = (ULONG) FindDisplayInfo(modeID)))
  {
    m_InitError = IE_GettingDisplayInfo;
    return false;
  }

  // Initialize the ColorMap, 3 planes deep, so 8 entries
  cm = GetColorMap(m_ViewNumColors);
  if (cm == NULL)
  {
    m_InitError = IE_GettingCM;
    return false;
  }

  /* Get ready to attach the ColorMap, Release 2-style */
  vcTags[0].ti_Data = (ULONG)&viewPort;

  /* Attach the color map and Release 2 extended structures */
  if (VideoControl(cm, vcTags))
  {
    m_InitError = IE_AttachExtStructs;
    return false;
  }

  // Construct preliminary Copper instruction list
  MakeVPort(&view, &viewPort);

  // Merge preliminary lists into a real Copper list in the View
  // structure
  MrgCop(&view);

  // Clear the ViewPort
  for (int depth = 0; depth < m_ViewDepth; depth++)
  {
    UBYTE* displaymem = (UBYTE*) bitMap1.Planes[depth];
    BltClear(displaymem, (bitMap1.BytesPerRow * bitMap1.Rows), 1L);
  }

  LoadView(&view);

  return true;
}

void GameView::FreeAll()
{
  // Put back the old view
  LoadView(oldview);

  // Befoire freeing memory wait until the old view is being  rendered
  WaitTOF();
  WaitTOF();

  // Deallocate the hardware Copper list created by MrgCop()
  FreeCprList(view.LOFCprList);

  // Free all intermediate Copper lists from created by MakeVPort()
  FreeVPortCopLists(&viewPort);

  //  Free the color map created by GetColorMap()
  if (cm != NULL)
  {
    FreeColorMap(cm);
  }

  // Free the ViewPortExtra created by GfxNew()
  if (vpextra != NULL)
  {
    GfxFree(vpextra);
  }

  //  Free the BitPlanes drawing area
  for (int depth = 0; depth < m_ViewDepth; depth++)
  {
    if (bitMap1.Planes[depth] != NULL)
    {
      FreeRaster(bitMap1.Planes[depth], m_ViewWidth, m_ViewHeight);
    }

    if (bitMap2.Planes[depth] != NULL)
    {
      FreeRaster(bitMap2.Planes[depth], m_ViewWidth, m_ViewHeight);
    }

  }

  // Free the MonitorSpec created with OpenMonitor()
  if (monspec != NULL)
  {
    CloseMonitor(monspec);
  }

  // Free the ViewExtra created with GfxNew()
  if (vextra)
  {
    GfxFree(vextra);
  }
}

struct RastPort* GameView::GetRastPort()
{
  return &rastPort;
}

struct ViewPort* GameView::GetViewPort()
{
  return &viewPort;
}

struct View* GameView::GetView()
{
  return &view;
}

void GameView::SwitchBuffers()
{
  if(m_BufToggle == false)
  {
    viewPort.RasInfo->BitMap = &bitMap2;
    rastPort.BitMap = &bitMap2;
    m_BufToggle = true;
  }
  else
  {
    viewPort.RasInfo->BitMap = &bitMap1;
    rastPort.BitMap = &bitMap1;
    m_BufToggle = false;
  }
}

const char* GameView::LastError() const
{
  switch(m_InitError)
  {
    case IE_None:
      return "GameView init successful.";
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
  }
}
