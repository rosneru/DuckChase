
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <graphics/videocontrol.h>

#include "lowlevelviewport.h"


LowlevelViewPort::LowlevelViewPort()
  : m_pViewPort(NULL),
    m_pViewPortExtra(NULL),
    m_pDisplayInfo(NULL)
{
}

LowlevelViewPort::~LowlevelViewPort()
{
  Delete();
}

bool LowlevelViewPort::Create(ULONG sizex, ULONG sizey, ULONG depth, 
                              ULONG modeId, ULONG colors, 
                              struct BitMap* pBitMap)
{
  if((1L << depth) > colors)
  {
    // Not enough colors in color map for the ViewPort's BitMap
    return false;
  }

  m_pViewPort = (struct ViewPort*) AllocVec(sizeof(struct ViewPort), 
                                            MEMF_CLEAR);

  if(m_pViewPort == NULL)
  {
    // Can't alocate memory for ViewPort
    Delete();
    return false;
  }

  InitVPort(m_pViewPort);

  m_pViewPort->ColorMap = GetColorMap(colors);
  if(m_pViewPort->ColorMap == NULL)
  {
    // Can't get ColorMap
    Delete();
    return false;
  }

  m_pViewPort->RasInfo = 
    (struct RasInfo*) AllocVec(sizeof(struct RasInfo), MEMF_CLEAR);
  
  if(m_pViewPort->RasInfo == NULL)
  {
    // Can't get RasInfo
    Delete();
    return false;
  }

  m_pViewPort->RasInfo->BitMap = pBitMap;

  m_pViewPortExtra = (struct ViewPortExtra*)
    GfxNew(VIEWPORT_EXTRA_TYPE);

  if(m_pViewPortExtra == NULL)
  {
    // Can't get ViewPortExtra
    Delete();
    return false;
  }

  m_pDisplayInfo = (struct DisplayInfo*)FindDisplayInfo(modeId);
  if(m_pDisplayInfo == NULL)
  {
    // Can't find DisplayInfo
    Delete();
    return false;
  }

  m_pViewPort->DWidth = sizex;
  m_pViewPort->DHeight = sizey;

  bool vcOk = VideoControlTags(m_pViewPort->ColorMap,
    VTAG_ATTACH_CM_SET, m_pViewPort,            // Attach ColorMap
    VTAG_VIEWPORTEXTRA_SET, m_pViewPortExtra,   // Attach VPExtra
    VTAG_NORMAL_DISP_SET, m_pDisplayInfo,
    VTAG_USERCLIP_SET, NULL,
    TAG_END);
  
  if(!vcOk)
  {
    // Can't set VideoControl
    Delete();
    return false;
  }

  LONG apeErr = AttachPalExtra(m_pViewPort->ColorMap, m_pViewPort);
  if(apeErr > 0)
  {
    return false;
  }

  return true;
}

void LowlevelViewPort::Delete()
{
  m_pDisplayInfo = NULL;

  if(m_pViewPortExtra != NULL)
  {
    GfxFree(m_pViewPortExtra);
    m_pViewPortExtra = NULL;
  }

  if(m_pViewPort != NULL)
  {
    if(m_pViewPort->RasInfo != NULL)
    {
      FreeVec(m_pViewPort->RasInfo);
      m_pViewPort->RasInfo = NULL;
    }

    if(m_pViewPort->ColorMap != NULL)
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