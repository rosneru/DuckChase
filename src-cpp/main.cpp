#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>



// characteristics of the pScreen
#define SCR_WIDTH  (640)
#define SCR_HEIGHT (256)
#define SCR_DEPTH    (3)


// Prototypes for our functions
VOID   runDBuff(struct Screen *, struct BitMap **);
struct BitMap **setupBitMaps(LONG, LONG, LONG);
VOID   freeBitMaps(struct BitMap **, LONG, LONG, LONG);
LONG   setupPlanes(struct BitMap *, LONG, LONG, LONG);
VOID    freePlanes(struct BitMap *, LONG, LONG, LONG);


/**
 * Main routine.  Setup for using the double buffered pScreen.
 * Clean up all resources when done or on any error.
 */
int main(int argc, char **argv)
{
  struct BitMap **pMyBitmapsArray;
  struct Screen  *pScreen;
  struct NewScreen myNewScreen;

  pMyBitmapsArray = setupBitMaps(SCR_DEPTH, SCR_WIDTH, SCR_HEIGHT);

  if (pMyBitmapsArray != NULL)
  {
    //
    // Open a simple quiet pScreen that is using the first of the two
    // bitmaps.
    //
    myNewScreen.LeftEdge = 0;
    myNewScreen.TopEdge = 0;
    myNewScreen.Width = SCR_WIDTH;
    myNewScreen.Height = SCR_HEIGHT;
    myNewScreen.Depth = SCR_DEPTH;
    myNewScreen.DetailPen = 0;
    myNewScreen.BlockPen = 1;
    myNewScreen.ViewModes = HIRES;
    myNewScreen.Type = CUSTOMSCREEN | CUSTOMBITMAP | SCREENQUIET;
    myNewScreen.Font = NULL;
    myNewScreen.DefaultTitle = NULL;
    myNewScreen.Gadgets = NULL;
    myNewScreen.CustomBitMap = pMyBitmapsArray[0];

    pScreen = OpenScreen(&myNewScreen);
    if (pScreen != NULL)
    {
      // Indicate that the rastport is double buffered.
      pScreen->RastPort.Flags = DBUFFER;

      runDBuff(pScreen, pMyBitmapsArray);

      CloseScreen(pScreen);
    }

    freeBitMaps(pMyBitmapsArray, SCR_DEPTH, SCR_WIDTH, SCR_HEIGHT);
  }
}

/**
 * Allocate the bit maps for a double buffered screen.
 */
struct BitMap** setupBitMaps(LONG depth, LONG width, LONG height)
{
  // this must be static -- it mustn't go away when the routine exits.
  static struct BitMap *pMyBitmapsArray[2];

  pMyBitmapsArray[0] = (struct BitMap *)
    AllocMem((LONG) sizeof(struct BitMap), MEMF_CLEAR);

  if (pMyBitmapsArray[0] != NULL)
  {
    pMyBitmapsArray[1] = (struct BitMap *)
      AllocMem((LONG) sizeof(struct BitMap), MEMF_CLEAR);

    if (pMyBitmapsArray[1] != NULL)
    {
      InitBitMap(pMyBitmapsArray[0], depth, width, height);
      InitBitMap(pMyBitmapsArray[1], depth, width, height);

      if (setupPlanes(pMyBitmapsArray[0], depth, width, height) != NULL)
      {
        if (setupPlanes(pMyBitmapsArray[1], depth, width, height) != NULL)
        {
          return pMyBitmapsArray;
        }

        freePlanes(pMyBitmapsArray[0], depth, width, height);
      }
      FreeMem(pMyBitmapsArray[1], (LONG) sizeof(struct BitMap));
    }
    FreeMem(pMyBitmapsArray[0], (LONG) sizeof(struct BitMap));
  }

  return NULL;
}

/**
 * Allocate the bit planes for a pScreen bit map
 */
LONG setupPlanes(struct BitMap *bitMap, LONG depth, LONG width, LONG height)
{
  SHORT plane_num;

  for (plane_num = 0; plane_num < depth; plane_num++)
  {
    bitMap->Planes[plane_num] = (PLANEPTR) AllocRaster(width, height);
    if (bitMap->Planes[plane_num] != NULL)
      BltClear(bitMap->Planes[plane_num], (width / 8) * height, 1);
    else
    {
      freePlanes(bitMap, depth, width, height);
      return(NULL);
    }
  }

  return(TRUE);
}

/**
 * Free the memory allocated by setupPlanes()
 */
VOID freePlanes(struct BitMap *bitMap, LONG depth, LONG width,
                LONG height)
{
  SHORT plane_num;

  for (plane_num = 0; plane_num < depth; plane_num++)
  {
    if (bitMap->Planes[plane_num] != NULL)
    {
      FreeRaster(bitMap->Planes[plane_num], width, height);
    }
  }
}


/**
 * Free the memory allocated by setupBitMaps().
 */
VOID freeBitMaps(struct BitMap **pMyBitmapsArray, LONG depth,
                 LONG width, LONG height)
{
  freePlanes(pMyBitmapsArray[0], depth, width, height);
  freePlanes(pMyBitmapsArray[1], depth, width, height);

  FreeMem(pMyBitmapsArray[0], (LONG) sizeof(struct BitMap));
  FreeMem(pMyBitmapsArray[1], (LONG) sizeof(struct BitMap));
}


/**
 * Loop through a number of iterations of drawing into alternate frames
 * of the double-buffered screen.  Note that the object is drawn in
 * color 1.
 */
VOID runDBuff(struct Screen *pScreen, struct BitMap **pMyBitmapsArray)
{
  WORD ktr, xpos, ypos;
  WORD toggleFrame;

  toggleFrame = 0;
  SetAPen(&(pScreen->RastPort), 1);

  for (ktr = 1; ktr < 2000; ktr++)
  {
    //
    // Calculate a position to place the object, these
    // calculations ensure the object will stay on the pScreen
    // given the range of ktr and the size of the object.
    //
    xpos = ktr;
    if ((ktr % 100) >= 50)
    {
      ypos = 50 - (ktr % 50);
    }
    else
    {
      ypos = ktr % 50;
    }

    // Switch the bitmap so that we are drawing into the correct place
    pScreen->RastPort.BitMap = pMyBitmapsArray[toggleFrame];
    pScreen->ViewPort.RasInfo->BitMap = pMyBitmapsArray[toggleFrame];

    //
    // Draw the objects.
    // Here we clear the old frame and draw a simple filled rectangle.
    //
    SetRast(&(pScreen->RastPort), 0);
    RectFill(&(pScreen->RastPort), xpos, ypos, xpos + 100, ypos + 100);

    // Update the physical display to match the newly drawn bitmap
    MakeScreen(pScreen);  // Tell intuition to do its stuff.
    RethinkDisplay();     // Intuition compatible MrgCop & LoadView,
                          // it also does a WaitTOF().

    // Switch the frame number for the next time through
    toggleFrame ^= 1;
  }
}
