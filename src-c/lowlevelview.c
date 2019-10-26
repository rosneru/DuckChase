/**
 * LowLevelView.c - Execute me to compile me with Lattice 6.2
 * sc data=near nominc strmer streq nostkchk saveds ign=73 LowLevelView
 * quit
 */
#include <exec/memory.h>
#include <exec/libraries.h>
#include <graphics/view.h>
#include <graphics/gfxbase.h>
#include <graphics/videocontrol.h>
#include <graphics/monitor.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxmacros.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include "lowlevelview.h"

/// View

struct View *CreateAView(APTR pMemoryPool, ULONG modeid)
{
  struct ViewExtra *ve;
  struct View *view = NULL;
  if (view = (struct View*)AllocPooled(pMemoryPool, sizeof(struct View)))
  {
    InitView(view);
    if (ve = (struct ViewExtra*) GfxNew(VIEW_EXTRA_TYPE)) /* For V36 and up, you need a ViewExtra if */
    {                                                     /* if you want to use a non-default monitor. */
      if (ve->Monitor = OpenMonitor(NULL, modeid))
      {
        GfxAssociate(view, ve);
        view->Modes |= EXTEND_VSTRUCT; /* Mark the View as having a ViewExtra. */
      }
      else
      {
        GfxFree(ve);
        ve = NULL;
      }
    }
    if (!ve)
    {
      view = NULL;
    }
  }

  return (view);
}

void DeleteAView(struct View *pView)
{
  struct ViewExtra *ve = NULL;
  if (ve = (struct ViewExtra *)GfxLookUp(pView))
  {
    if (ve->Monitor)
    {
      CloseMonitor(ve->Monitor);
    }

    GfxFree(ve);
  }

  if(pView != NULL)
  {
    if(pView->LOFCprList != NULL)
    {
      // Deallocate the hardware Copper list created by MrgCop()
      FreeCprList(pView->LOFCprList);
      pView->LOFCprList = NULL;
    }

    if(pView->SHFCprList != NULL)
    {
      // Deallocate also the interlace-only hardware Copper list
      FreeCprList(pView->SHFCprList);
      pView->SHFCprList = NULL;
    }
  }
}

///

/// ViewPort

struct ViewPort *CreateAViewPort(APTR pMemoryPool,
                                 ULONG sizex,
                                 ULONG sizey,
                                 ULONG depth,
                                 ULONG modeid,
                                 ULONG colors)
{
  struct ViewPort *pViewPort = NULL;
  struct ViewPortExtra *vpextra = NULL;
  struct DisplayInfo *disinfo = NULL;
  struct DimensionInfo dimquery = {0};

  if ((1L<<depth) <= colors)  /* There must be enough colors in the */
  {                           /* colormap for the ViewPort’s bitmap. */
    if (pViewPort = (struct ViewPort*) AllocPooled(pMemoryPool, sizeof(struct ViewPort)))
    {
      InitVPort(pViewPort); /* This function clears some of the   */
                            /* ViewPort field so call it before   */
                            /* initializing any ViewPort fields.  */
      if (pViewPort->ColorMap = GetColorMap(colors))
      {
        /* I need a ColorMap if I want to use certain */
        /* VideoControl() features.                   */
        if (pViewPort->RasInfo = (struct RasInfo *)
                AllocPooled(pMemoryPool, sizeof(struct RasInfo)))
        {
          if (pViewPort->RasInfo->BitMap =
                  AllocBitMap(sizex, sizey, depth,
                              BMF_DISPLAYABLE | BMF_CLEAR,
                              NULL))
          {
            if (vpextra = (struct ViewPortExtra*) GfxNew(VIEWPORT_EXTRA_TYPE))
            {
              if (disinfo = (struct DisplayInfo*)FindDisplayInfo(modeid))
              {

                if(GetDisplayInfoData(disinfo, (UBYTE*) &dimquery,
                                      sizeof(dimquery), DTAG_DIMS, modeid))
                {
                  vpextra->DisplayClip = dimquery.Nominal;
                }

                pViewPort->DWidth = sizex;
                pViewPort->DHeight = sizey;

                struct TagItem vcTags[] =
                {
                  /* This tag associates a ColorMap with a ViewPort. */
                  {VTAG_ATTACH_CM_SET, (ULONG)pViewPort },
                  /* This tag associates a ViewPortExtra with a      */
                  /* ViewPort. Notice that VideoControl() (*not*     */
                  /* GfxAssociate()) associates the VP and VPE.      */
                  {VTAG_VIEWPORTEXTRA_SET, (ULONG)vpextra},
                  {VTAG_NORMAL_DISP_SET, (ULONG)disinfo},
                  {VTAG_USERCLIP_SET, NULL},
                  {TAG_END}
                };

                if (!VideoControl(pViewPort->ColorMap, vcTags))
                {
                  if (AttachPalExtra(pViewPort->ColorMap, pViewPort))
                    disinfo = NULL;
                }
              }

              if (!disinfo)
              {
                GfxFree(vpextra);
                vpextra = NULL;
              }
            }

            if (!vpextra)
            {
              FreeBitMap(pViewPort->RasInfo->BitMap);
              pViewPort->RasInfo->BitMap = NULL;
            }
          }

          if (!(pViewPort->RasInfo->BitMap))
          {
            pViewPort->RasInfo = NULL;
          }
        }

        if (!(pViewPort->RasInfo))
        {
          FreeColorMap(pViewPort->ColorMap);
          pViewPort->ColorMap = NULL;
        }
      }

      if (!(pViewPort->ColorMap))
      {
        pViewPort = NULL;
      }
    }
  }

  return (pViewPort);
}

void DeleteAViewPort(struct ViewPort *pViewPort)
{
  struct TagItem ti[2];
  ti[0].ti_Tag = VTAG_VIEWPORTEXTRA_GET;
  ti[0].ti_Data = NULL; /* VideoControl() will write over this with */
                        /* ViewPortExtra address.                   */
  ti[1].ti_Tag = VTAG_END_CM;

  /* Can't use VideoControlTags() here. */
  VideoControl(pViewPort->ColorMap, ti);

  if (ti[0].ti_Data)
  {
    /* Free the ViewPortExtra, if it exists. */
    GfxFree((APTR)ti[0].ti_Data);
  }

  FreeBitMap(pViewPort->RasInfo->BitMap);
  FreeColorMap(pViewPort->ColorMap);
}

///
