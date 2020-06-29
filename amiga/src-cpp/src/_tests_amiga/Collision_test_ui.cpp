/**
 * Picture_test - tests the method Picture::GetBitMapPixelColorNum()
 *
 * Loading and blitting a LoRes RAW picture.
 *
 * Then while you hover with the mouse over the pixels the current
 * pixels color number is displayed in a gasget at the top
 *
 *
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


struct Screen* m_pScreen = NULL;
struct Window* m_pWindow = NULL;

char charBuf[80];

void intuiEventLoop();
int cleanExit(int errorCode = 0);


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

  // if(m_BackgroundImage.LoadFromRawFile("gfx/screen_m25.raw",
  //                                      223, 233, 4) == false)
  // {
  //   printf("Failed to load gfx/screen_m25.raw.\n");
  //   cleanExit(RETURN_FAIL);
  // }

  // // size_t imgYOffset = 5;
  // BltBitMapRastPort(m_BackgroundImage.GetBitMap(),
  //                   0,
  //                   0,
  //                   m_pWindow->RPort,
  //                   BLT_XSTRT,
  //                   BLT_YSTRT,
  //                   BLT_XWDTH,
  //                   BLT_YWDTH,
  //                   0xC0);
  

  intuiEventLoop();
  cleanExit(RETURN_OK);
}

void intuiEventLoop()
{
  struct IntuiMessage* pMsg;

  int x = 0;
  int y = 0;

  while(true)
  {
    Wait(1L << m_pWindow->UserPort->mp_SigBit);
    while (NULL != (pMsg = (struct IntuiMessage*)GetMsg(m_pWindow->UserPort)))
    {
      // only CLOSEWINDOW and INTUITICKS are active
      if (pMsg->Class == CLOSEWINDOW)
      {
        ReplyMsg((struct Message*)pMsg);
        return;
      }

      // Handle the intuiticks message

      x = pMsg->MouseX - 4;
      y = pMsg->MouseY - 16;

      ReplyMsg((struct Message*)pMsg);

      // TODO Fixme
      //int colNum = m_BackgroundImage.GetBitMapPixelColorNum(y, x);
      int colNum = 666;

      sprintf(charBuf, "x = %d, y = %d : color number = %d", x, y, colNum);
      SetWindowTitles(m_pWindow, charBuf, (STRPTR) ~0);
    }
  }
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
