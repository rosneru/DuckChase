#include <exec/types.h>
#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <graphics/copper.h>
#include <graphics/view.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxnodes.h>
#include <graphics/videocontrol.h>
#include <libraries/dos.h>
#include <libraries/lowlevel.h>
#include <utility/tagitem.h>

#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/lowlevel_protos.h>

#include "animtools.h"
#include "animtools_proto.h"

#include "stdiostring.h"

#include "Picture.h"
#include "GelsBob.h"

// Dimensions of the view
#define DEPTH     (3)
#define NUMCOLORS (8)
#define WIDTH     (640)
#define HEIGHT    (256)

void cleanup(int);
void fail(STRPTR);

extern struct GfxBase* GfxBase;

//These are global to make freeing easier.
struct View view;
struct ViewPort viewPort = { 0 };
struct BitMap bitMap = { 0 };
struct RastPort rastPort;

struct ColorMap *cm = NULL;

// Extended structures used in Release 2
struct ViewExtra *vextra = NULL;
struct MonitorSpec *monspec = NULL;
struct ViewPortExtra *vpextra = NULL;
struct DimensionInfo dimquery = { 0 };

Picture picBackgr;



int main(void)
{
  SetJoyPortAttrs(1,
                  SJA_Type, SJA_TYPE_AUTOSENSE,
                  TAG_END);

  SystemControl(SCON_TakeOverSys, TRUE,
                TAG_END);


  // Pointer to old View we can restore it.
  struct View *oldview = NULL;

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
    fail("Could not get ViewExtra\n");
  }

  // Attach the ViewExtra to the View
  GfxAssociate(&view, vextra);
  view.Modes |= EXTEND_VSTRUCT;

  // Create and attach a MonitorSpec to the ViewExtra
  monspec = OpenMonitor(NULL, modeID);
  if (monspec == NULL)
  {
    fail("Could not get MonitorSpec\n");
  }

  vextra->Monitor = monspec;

  // Initialize the BitMap for RasInfo
  InitBitMap(&bitMap, DEPTH, WIDTH, HEIGHT);

  // Set the plane pointers to NULL so the cleanup routine
  // will know if they were used
  for (int depth = 0; depth < DEPTH; depth++)
    bitMap.Planes[depth] = NULL;

  // Allocate space for BitMap
  for (int depth = 0; depth < DEPTH; depth++)
  {
    bitMap.Planes[depth] = (PLANEPTR) AllocRaster(WIDTH, HEIGHT);
    if (bitMap.Planes[depth] == NULL)
    {
      fail("Could not get BitPlanes\n");
    }
  }

  // Create a RstPort to draw into
  InitRastPort(&rastPort);
  rastPort.BitMap = &bitMap;
  SetRast(&rastPort, 0);

  // Initialize the RasInfo
  rasInfo.BitMap = &bitMap;
  rasInfo.RxOffset = 0;
  rasInfo.RyOffset = 0;
  rasInfo.Next = NULL;

  // Initialize the ViewPort
  InitVPort(&viewPort);
  view.ViewPort = &viewPort; // Link the ViewPort into the View
  viewPort.RasInfo = &rasInfo;
  viewPort.DWidth = WIDTH;
  viewPort.DHeight = HEIGHT;

  // Make a ViewPortExtra and get ready to attach it
  vpextra = (struct ViewPortExtra*) GfxNew(VIEWPORT_EXTRA_TYPE);

  if (vpextra == NULL)
  {
    fail("Could not get ViewPortExtra\n");
  }

  vcTags[1].ti_Data = (ULONG)vpextra;

  // Initialize the DisplayClip field of the ViewPortExtra
  if (GetDisplayInfoData(NULL, (UBYTE *)&dimquery,
    sizeof(dimquery), DTAG_DIMS, modeID) == 0)
  {
    fail("Could not get DimensionInfo \n");
  }

  vpextra->DisplayClip = dimquery.Nominal;

  // Make a DisplayInfo and get ready to attach it
  if (!(vcTags[2].ti_Data = (ULONG) FindDisplayInfo(modeID)))
  {
    fail("Could not get DisplayInfo\n");
  }



  // Initialize the ColorMap, 3 planes deep, so 8 entries
  cm = GetColorMap(NUMCOLORS);
  if (cm == NULL)
  {
    fail("Could not get ColorMap\n");
  }

  /* Get ready to attach the ColorMap, Release 2-style */
  vcTags[0].ti_Data = (ULONG)&viewPort;

  /* Attach the color map and Release 2 extended structures */
  if (VideoControl(cm, vcTags))
  {
    fail("Could not attach extended structures\n");
  }


  // Construct preliminary Copper instruction list
  MakeVPort(&view, &viewPort);

  // Merge preliminary lists into a real Copper list in the View
  // structure
  MrgCop(&view);

  // Clear the ViewPort
  for (int depth = 0; depth < DEPTH; depth++)
  {
    UBYTE* displaymem = (UBYTE*) bitMap.Planes[depth];
    BltClear(displaymem, (bitMap.BytesPerRow * bitMap.Rows), 1L);
  }

  LoadView(&view);

  //
  // Setting the used color table (extracted from pic wit BtoC32)
  //
  USHORT colortable[8] = {
    0xAAA, 0x0, 0xFFF, 0x68B, 0x5A3, 0xEB0, 0xB52, 0xF80
  };

  // Change colors to those in colortable
  LoadRGB4(&viewPort, colortable, NUMCOLORS);


  //
  // Loading background image
  //
  if(picBackgr.LoadFromRawFile("/gfx/background_hires.raw",
                               WIDTH, HEIGHT, DEPTH) == FALSE)
  {
    fail("Could not load background image\n");
  }

  BltBitMapRastPort(picBackgr.GetBitMap(), 0, 0, &rastPort,
                    0, 0, WIDTH, HEIGHT, 0xC0);




  Delay(10L * TICKS_PER_SECOND);

  WaitTOF();
  WaitTOF();

  // Put back the old view
  LoadView(oldview);

  // Befoire freeing memory wait until the old view is being  rendered
  WaitTOF();
  WaitTOF();

  // Deallocate the hardware Copper list created by MrgCop()
  FreeCprList(view.LOFCprList);

  // Free all intermediate Copper lists from created by MakeVPort()
  FreeVPortCopLists(&viewPort);

  cleanup(RETURN_OK);
}


/**
 * fail():  print the error string and call cleanup() to exit
 */
void fail(STRPTR errorstring)
{
  VPrintf(errorstring, NULL);
  cleanup(RETURN_FAIL);
}

/**
 * Fee everything that was allocated
 */
void cleanup(int returncode)
{
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
  for (int depth = 0; depth < DEPTH; depth++)
  {
    if (bitMap.Planes[depth] != NULL)
    {
      FreeRaster(bitMap.Planes[depth], WIDTH, HEIGHT);
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

  SystemControl(SCON_TakeOverSys, FALSE,
                TAG_END);


  exit(returncode);
}
