/**
 * Tests the method collision detection algorithm of the framework.
 * 
 * Move the mouse with the arrow attached to it. As soon as it is inside
 * the duck rectangle dimesnions this should be displayed on the screen
 * title bar. As soon as there's a collsision of non-background-colored
 * pixels this should also be displayed on the title bar.
 *
 * 30.06.2020
 * Author: Uwe Rosner https://github.com/rosneru
 */
#include <stdlib.h>
#include <stdio.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#include "GelsLayer.h"
#include "ArrowResources.h"
#include "DuckResources.h"
#include "ShapeExtSprite.h"
#include "ShapeBob.h"
#include "ShadowMask.h"

struct Screen* m_pScreen = NULL;
struct Window* m_pWindow = NULL;

void intuiEventLoop(ShapeBase& duck, 
                    const ShadowMask* pDuckMask,
                    ShapeBase& arrow, 
                    const ShadowMask* pArrowMask);

void displayCollisionState();

int cleanExit(int errorCode = 0);

enum CollisionState
{
  CollisionNone,
  CollisionRects,
  CollisionFull
};

CollisionState m_FormerCollisionState = CollisionNone;

int main(int argc, char** argv)
{
  m_pScreen = OpenScreenTags(NULL,
    SA_LikeWorkbench, TRUE,
    SA_DisplayID, PAL_MONITOR_ID|LORES_KEY,
    SA_Depth, 4,
    SA_Width, 320,
    SA_Height, 256,
    SA_Title, "Not colliding..",
    SA_Type, CUSTOMSCREEN,
    SA_Exclusive, TRUE,
    TAG_DONE);
  
  if(m_pScreen == NULL)
  {
    printf("Failed to open screen.\n");
    cleanExit(RETURN_FAIL);
  }

  m_pWindow = OpenWindowTags(NULL,
    WA_CustomScreen, m_pScreen,
    WA_Left, 0,
    WA_Top, 12,
    WA_Width, 600,
    WA_Height, 200,
    WA_Title, "",
    WA_SmartRefresh, TRUE,
    WA_NewLookMenus, TRUE,
    WA_Flags, WFLG_ACTIVATE|WFLG_CLOSEGADGET|WFLG_DRAGBAR|WFLG_GIMMEZEROZERO,
    WA_IDCMP, IDCMP_CLOSEWINDOW|IDCMP_INTUITICKS,
    TAG_DONE);
  
  if(m_pWindow == NULL)
  {
    printf("Failed to open window.\n");
    cleanExit(RETURN_FAIL);
  }

  GelsLayer gelsLayer(m_pWindow->RPort, 0xff);

  DuckResources duckResources;
  ArrowResources arrowResources;

  ShapeExtSprite arrowShape(&m_pScreen->ViewPort, arrowResources);
  ShapeBob duckShape(m_pWindow->RPort, 4, duckResources);

  const ShadowMask* pArrowMask = arrowResources.AnimRightUpward()->Mask(0);
  const ShadowMask* pDuckMask = duckResources.AnimFlyLeft()->Mask(0);

  duckShape.Move(50, 50);
  arrowShape.Move(160, 128);

  displayCollisionState();
 

  intuiEventLoop(duckShape, pDuckMask, arrowShape, pArrowMask);
  cleanExit(RETURN_OK);
}

void intuiEventLoop(ShapeBase& duck, 
                    const ShadowMask* pDuckMask,
                    ShapeBase& arrow, 
                    const ShadowMask* pArrowMask)
{
  struct IntuiMessage* pMsg;

  int x = 0;
  int y = 0;

  while(true)
  {
    Wait(1L << m_pWindow->UserPort->mp_SigBit);
    while (NULL != (pMsg = (struct IntuiMessage*)GetMsg(m_pWindow->UserPort)))
    {
      
      if (pMsg->Class == CLOSEWINDOW)
      {
        //
        // Handle CLOSEWINDOW
        //
        ReplyMsg((struct Message*)pMsg);
        return;
      }

      //
      // This must be INTUITICKS as only CLOSEWINDOW and INTUITICKS are 
      // active
      //

      x = pMsg->MouseX - 4;
      y = pMsg->MouseY - 16;

      ReplyMsg((struct Message*)pMsg);

      arrow.Move(x, y);

      // Render the gels
      SortGList(m_pWindow->RPort);
      DrawGList(m_pWindow->RPort, &m_pScreen->ViewPort);
      WaitTOF();

      CollisionState newCollisionState = CollisionNone;
      if(arrow.Intersects(duck))
      {
        newCollisionState = CollisionRects;

        duck.Intersects(arrow);
        
        if(pArrowMask->IsCollision(pDuckMask,
                                   arrow.IntersectRect(),
                                   duck.IntersectRect()))
        {
          newCollisionState = CollisionFull;
        }
      }

      if(newCollisionState != m_FormerCollisionState)
      {
        m_FormerCollisionState = newCollisionState;
        displayCollisionState();
      }
    }
  }
}


void displayCollisionState()
{
  char charBuf[80];

  switch (m_FormerCollisionState)
  {
  case CollisionNone:
    sprintf(charBuf, "No collision");
    break;

  case CollisionRects:
    sprintf(charBuf, ">> Rect collision");
    break;

  case CollisionFull:
    sprintf(charBuf, ">>>> FULL COLLISSION!!");
    break;
  
  default:
    break;
  }
  
  SetWindowTitles(m_pWindow, charBuf, (STRPTR) ~0);
}

int cleanExit(int errorCode)
{
  if(m_pWindow != NULL)
  {
    CloseWindow(m_pWindow);
    m_pWindow = NULL;
  }

  if(m_pScreen != NULL)
  {
    CloseScreen(m_pScreen);
    m_pScreen = NULL;
  }

  exit(errorCode);
}
