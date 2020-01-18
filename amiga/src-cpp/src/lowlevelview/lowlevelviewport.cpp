
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <graphics/videocontrol.h>

#include "lowlevelviewport.h"

LowlevelViewPort::LowlevelViewPort()
  : m_pViewPort(NULL), 
    m_pViewPortExtra(NULL)
{
}

LowlevelViewPort::~LowlevelViewPort()
{
  Delete();
}

bool LowlevelViewPort::Create(ULONG sizex, 
                              ULONG sizey, 
                              ULONG depth, 
                              ULONG modeId, 
                              ULONG colors, 
                              struct BitMap* pBitMap)
{
  m_InitError = IE_None;

  if ((1UL << depth) > colors)
  {
    // Not enough colors in color map for the ViewPort's BitMap
    m_InitError = IE_ColorMapTooSmall;
    return false;
  }

  m_pViewPort = (struct ViewPort*)AllocVec(sizeof(struct ViewPort), MEMF_CLEAR);

  if (m_pViewPort == NULL)
  {
    // Can't alocate memory for ViewPort
    m_InitError = IE_GettingViewportMem;
    Delete();
    return false;
  }

  InitVPort(m_pViewPort);

  m_pViewPort->ColorMap = GetColorMap(colors);
  if (m_pViewPort->ColorMap == NULL)
  {
    // Can't get ColorMap
    m_InitError = IE_GettingColorMap;
    Delete();
    return false;
  }

  m_pViewPort->RasInfo =
    (struct RasInfo*)AllocVec(sizeof(struct RasInfo), MEMF_CLEAR);

  if (m_pViewPort->RasInfo == NULL)
  {
    // Can't get RasInfo
    m_InitError = IE_GettingRasInfo;
    Delete();
    return false;
  }

  m_pViewPort->RasInfo->BitMap = pBitMap;

  m_pViewPortExtra = (struct ViewPortExtra*)GfxNew(VIEWPORT_EXTRA_TYPE);

  if (m_pViewPortExtra == NULL)
  {
    // Can't get ViewPortExtra
    m_InitError = IE_GettingVPExtra;
    Delete();
    return false;
  }

  m_pViewPort->DWidth = sizex;
  m_pViewPort->DHeight = sizey;
  m_pViewPort->Modes = modeId;

  struct TagItem vcTags[] = {{VTAG_ATTACH_CM_SET, (ULONG)m_pViewPort},
                             {VTAG_VIEWPORTEXTRA_SET, (ULONG)m_pViewPortExtra},
                             {VTAG_SPRITERESN_SET, SPRITERESN_70NS},
                             {TAG_DONE}};

  VideoControl(m_pViewPort->ColorMap, vcTags);
  return true;
}

void LowlevelViewPort::Delete()
{
  if (m_pViewPortExtra != NULL)
  {
    GfxFree(m_pViewPortExtra);
    m_pViewPortExtra = NULL;
  }

  if (m_pViewPort != NULL)
  {
    if (m_pViewPort->RasInfo != NULL)
    {
      FreeVec(m_pViewPort->RasInfo);
      m_pViewPort->RasInfo = NULL;
    }

    if (m_pViewPort->ColorMap != NULL)
    {
      FreeColorMap(m_pViewPort->ColorMap);
      m_pViewPort->ColorMap = NULL;
    }

    FreeVec(m_pViewPort);
    m_pViewPort = NULL;
  }
}

struct ViewPort* LowlevelViewPort::ViewPort()
{
  return m_pViewPort;
}

const char* LowlevelViewPort::LastError() const
{
  switch (m_InitError)
  {
  case IE_None:
    return "No error: init done successfully.\n";
    break;

  case IE_ColorMapTooSmall:
    return "ColorMap is too small for BitMap depth.\n";
    break;

  case IE_GettingViewportMem:
    return "Can't allocate ViewPort memory.\n";
    break;

  case IE_GettingColorMap:
    return "Can't get ColorMap.\n";
    break;

  case IE_GettingRasInfo:
    return "Can't get RasInfo.\n";
    break;

  case IE_GettingVPExtra:
    return "Can't get ViewPortExtra.\n";
    break;

  case IE_GettingDisplayInfo:
    return "Can't get DisplayInfo.\n";
    break;

  default:
    return "Unknown error in LowlevelView.\n";
    break;
  }
}
