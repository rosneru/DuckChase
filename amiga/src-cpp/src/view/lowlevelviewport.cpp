
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <graphics/videocontrol.h>

#include <stddef.h>

#include "lowlevelviewport.h"

LowlevelViewPort::LowlevelViewPort(ULONG sizex,
                                   ULONG sizey,
                                   ULONG depth,
                                   ULONG modeId,
                                   ULONG colors,
                                   struct BitMap* pBitMap,
                                   const ULONG* pColorArray)
  : m_pViewPort(NULL),
    m_pViewPortExtra(NULL)
{
  if ((1UL << depth) > colors)
  {
    throw "LowlevelViewPort: Not enough colors in color map for the ViewPort's BitMap.";
  }

  m_pViewPort = (struct ViewPort*) AllocVec(sizeof(struct ViewPort),
                                            MEMF_CLEAR);

  if (m_pViewPort == NULL)
  {
    throw "LowlevelViewPort: Can't alocate memory for ViewPort";
  }

  InitVPort(m_pViewPort);

  m_pViewPort->ColorMap = GetColorMap(colors);
  if (m_pViewPort->ColorMap == NULL)
  {
    throw "LowlevelViewPort: Can't get ColorMap.";
  }

  size_t rasInfoSize = sizeof(struct RasInfo);
  m_pViewPort->RasInfo = (struct RasInfo*) AllocVec(rasInfoSize,
                                                    MEMF_CLEAR);

  if (m_pViewPort->RasInfo == NULL)
  {
    throw "LowlevelViewPort: Can't get RasInfo.";
  }

  m_pViewPort->RasInfo->BitMap = pBitMap;

  m_pViewPortExtra = (struct ViewPortExtra*)GfxNew(VIEWPORT_EXTRA_TYPE);

  if (m_pViewPortExtra == NULL)
  {
    throw "LowlevelViewPort: Can't get ViewPortExtra.";
  }

  m_pViewPort->DWidth = sizex;
  m_pViewPort->DHeight = sizey;
  m_pViewPort->Modes = modeId;

  struct TagItem vcTags[] = {{VTAG_ATTACH_CM_SET, (ULONG)m_pViewPort},
                             {VTAG_VIEWPORTEXTRA_SET, (ULONG)m_pViewPortExtra},
//                             {VTAG_SPRITERESN_SET, SPRITERESN_140NS},
                             {TAG_DONE}};

  VideoControl(m_pViewPort->ColorMap, vcTags);

  if(pColorArray != NULL)
  {
    LoadRGB32(m_pViewPort, pColorArray);
  }

}

LowlevelViewPort::~LowlevelViewPort()
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
