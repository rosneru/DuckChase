#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfx.h>
#include <graphics/gels.h>
#include <graphics/clip.h>
#include <graphics/rastport.h>
#include <graphics/view.h>
#include <graphics/gfxbase.h>

#include "GelsLayer.h"


GelsLayer::GelsLayer(struct RastPort* pRastPort, BYTE reserved)
  : m_pRastPort(pRastPort)
{
  if(m_pRastPort == NULL)
  {
    throw "GelsLayer: Empty RastPort provided.";
  }

  struct VSprite  *vsHead;
  struct VSprite  *vsTail;

  if (NULL != (m_pGelsInfo = (struct GelsInfo *) AllocMem(sizeof(struct GelsInfo), MEMF_CLEAR)))
  {
    if (NULL != (m_pGelsInfo->nextLine = (WORD *) AllocMem(sizeof(WORD) * 8, MEMF_CLEAR)))
    {
      if (NULL != (m_pGelsInfo->lastColor = (WORD **) AllocMem(sizeof(LONG) * 8, MEMF_CLEAR)))
      {
        if (NULL != (m_pGelsInfo->collHandler = (struct collTable *) AllocMem(sizeof(struct collTable),MEMF_CLEAR)))
        {
          if (NULL != (vsHead = (struct VSprite *) AllocMem((LONG)sizeof(struct VSprite), MEMF_CLEAR)))
          {
            if (NULL != (vsTail = (struct VSprite *) AllocMem(sizeof(struct VSprite), MEMF_CLEAR)))
            {
              m_pGelsInfo->sprRsrvd   = reserved;
              /* Set left- and top-most to 1 to better keep items */
              /* inside the display boundaries.                   */
              m_pGelsInfo->leftmost   = m_pGelsInfo->topmost    = 1;
              m_pGelsInfo->rightmost  = (m_pRastPort->BitMap->BytesPerRow << 3) - 1;
              m_pGelsInfo->bottommost = m_pRastPort->BitMap->Rows - 1;
              m_pRastPort->GelsInfo = m_pGelsInfo;
              InitGels(vsHead, vsTail, m_pGelsInfo);
              return;
            }

            FreeMem(vsHead, (LONG)sizeof(*vsHead));
          }

          FreeMem(m_pGelsInfo->collHandler, (LONG)sizeof(struct collTable));
        }

        FreeMem(m_pGelsInfo->lastColor, (LONG)sizeof(LONG) * 8);
      }

      FreeMem(m_pGelsInfo->nextLine, (LONG)sizeof(WORD) * 8);
    }

    FreeMem(m_pGelsInfo, (LONG)sizeof(*m_pGelsInfo));
  }

  throw "GelsLayer: Failed to initialize GELs system.";
}


GelsLayer::~GelsLayer()
{
  if(m_pRastPort != NULL)
  {
    m_pRastPort->GelsInfo = NULL;
  }

  if(m_pGelsInfo != NULL)
  {
    FreeMem(m_pGelsInfo->collHandler, (LONG)sizeof(struct collTable));
    FreeMem(m_pGelsInfo->lastColor, (LONG)sizeof(LONG) * 8);
    FreeMem(m_pGelsInfo->nextLine, (LONG)sizeof(WORD) * 8);
    FreeMem(m_pGelsInfo->gelHead, (LONG)sizeof(struct VSprite));
    FreeMem(m_pGelsInfo->gelTail, (LONG)sizeof(struct VSprite));
    FreeMem(m_pGelsInfo, (LONG)sizeof(*m_pGelsInfo));
  }
}
