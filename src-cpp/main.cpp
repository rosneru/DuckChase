#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <exec/memory.h>

#include <graphics/modeid.h>

#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <libraries/lowlevel.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/lowlevel_protos.h>

#include "animtools.h"
#include "animtools_proto.h"
#include "Picture.h"
#include "GelsBob.h"
#include "StopWatch.h"


/* A utility function to reverse a string  */
void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        char tmp = *(str+start);
        *(str+start) = *(str+end);
        *(str+end) = tmp;

        start++;
        end--;
    }
}

// Implementation of itoa()
char* itoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}


size_t strlen( const char *s )
{
    const char *p = s;

    while ( *p ) ++p;

    return p - s;
}


void bobDrawGList(struct RastPort *rport,
                  struct ViewPort *vport);


int main(int argc, char **argv)
{
  SetJoyPortAttrs(1,
                  SJA_Type, SJA_TYPE_AUTOSENSE,
                  TAG_END);

  UWORD pens[] = { ~0 };

  struct Screen* pScreen = OpenScreenTags(NULL,
    SA_Pens, pens,
    SA_Depth, 3,
    SA_Top, 0,
    SA_Left, 0,
    SA_Width, 640,
    SA_Height, 256,
    SA_DisplayID, PAL_MONITOR_ID|HIRES_KEY,
    SA_Quiet, TRUE,
    SA_Exclusive, TRUE,
    SA_Interleaved, TRUE,
    TAG_END);

  if (pScreen != NULL)
  {
    //
    // Setting the used color table (extracted from pic wit BtoC32)
    //
    UBYTE colArr[8][3] =
    {
      {0xA,0xA,0xA}, {0x0,0x0,0x0}, {0xF,0xF,0xF}, {0x6,0x8,0xB},
      {0x5,0xA,0x3}, {0xE,0xB,0x0}, {0xB,0x5,0x2}, {0xF,0x8,0x0}
    };

    for(int i = 0; i < 8; i++)
    {
      SetRGB4(&pScreen->ViewPort, i,
              colArr[i][0], colArr[i][1], colArr[i][2]);
    }

    //
    // Loading background image
    //
    Picture picBackgr;
    bool bOk = picBackgr.LoadFromRawFile("/gfx/background_hires.raw",
                                         640, 256, 3);

    if(bOk)
    {
      BltBitMapRastPort(picBackgr.GetBitMap(), 0, 0, &pScreen->RastPort,
                        0, 0, 640, 256, 0xC0);
    }



    struct GelsInfo* pGelsInfo;

    if ((pGelsInfo = setupGelSys(&pScreen->RastPort, 0x03)) != NULL)
    {
      GelsBob bobDuck(pScreen, 59, 21, 3);
      bobDuck.LoadImgFromRawFile("/gfx/ente1_hires.raw");
      bobDuck.LoadImgFromRawFile("/gfx/ente2_hires.raw");

      GelsBob bobHunter(pScreen, 16, 22, 3);
      bobHunter.LoadImgFromRawFile("/gfx/jaeger1_hires.raw");
      bobHunter.LoadImgFromRawFile("/gfx/jaeger2_hires.raw");

      struct Bob* pBobDuck = bobDuck.Get();
      struct Bob* pBobHunter = bobHunter.Get();

      if((pBobDuck != NULL) && (pBobHunter != NULL))
      {
        pBobDuck->BobVSprite->X = 200;
        pBobDuck->BobVSprite->Y = 40;

        pBobHunter->BobVSprite->X = 20;
        pBobHunter->BobVSprite->Y = 220;

        AddBob(pBobDuck, &pScreen->RastPort);
        AddBob(pBobHunter, &pScreen->RastPort);

        bobDrawGList(&pScreen->RastPort, &pScreen->ViewPort);

        bool bContinue = true;
        char pFpsBuf[] = {"FPS: __________"};
        char* pFpsNumberStart = pFpsBuf + 5;

        StopWatch stopWatch;
        stopWatch.Start();

        do
        {
          ULONG key = GetKey();
          if((key & 0x00ff) == 0x45) // RAW code ESC key
          {
            bContinue = false;
          }

          ULONG portState = ReadJoyPort(1);
          if((portState & JP_TYPE_MASK) == JP_TYPE_JOYSTK)
          {
            if((portState & JPF_JOY_RIGHT) != 0)
            {
              pBobHunter->BobVSprite->X += 8;
              if(pBobHunter->BobVSprite->X > 640)
              {
                pBobHunter->BobVSprite->X = -16;
              }
            }
            else if((portState & JPF_JOY_LEFT) != 0)
            {
              pBobHunter->BobVSprite->X -= 8;
              if(pBobHunter->BobVSprite->X < 0)
              {
                pBobHunter->BobVSprite->X = 656;
              }
            }
          }

          pBobDuck->BobVSprite->X -= 4;
          if(pBobDuck->BobVSprite->X < -40)
          {
            pBobDuck->BobVSprite->X = 650;
          }

          bobDuck.NextImage();
          InitMasks(pBobDuck->BobVSprite);
          bobDrawGList(&pScreen->RastPort, &pScreen->ViewPort);

          //
          // Display the FPS value
          //
          double dblElapsed = stopWatch.Pick();

          if(dblElapsed >= 0)
          {
            // Calculatin fps and writing it to the string buf
            short fps = 1000 / dblElapsed;
            itoa(fps, pFpsNumberStart, 10);

            SetBPen(&pScreen->RastPort, 0);
            EraseRect(&pScreen->RastPort, 52, 62, 130, 72);

            Move(&pScreen->RastPort, 50, 70);
            Text(&pScreen->RastPort, pFpsBuf, strlen(pFpsBuf));
          }
        }
        while (bContinue);

        RemBob(pBobHunter);
        RemBob(pBobDuck);

        bobDrawGList(&pScreen->RastPort, &pScreen->ViewPort);
      }

      cleanupGelSys(pGelsInfo, &pScreen->RastPort);
    }

    CloseScreen(pScreen);
  }
}

/**
 * Draw the Bobs into the RastPort.
 */
void bobDrawGList(struct RastPort *rport, struct ViewPort *vport)
{
  SortGList(rport);
  DrawGList(rport, vport);

  // If the GelsList includes true VSprites, MrgCop() and LoadView()
  // here
  WaitTOF() ;
}
