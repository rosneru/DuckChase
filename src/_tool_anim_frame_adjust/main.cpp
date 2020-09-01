#include <dos/dos.h>
#include <graphics/modeid.h>

#include <stdio.h>


#include "AnimFrameTool.h"

int main(void)
{
  try
  {
    // Create and run the game
    AnimFrameTool animFrameTool;
    animFrameTool.Run();

    return RETURN_OK;
  }
  catch(const char* pMsg)
  {
    printf("%s\n", pMsg);
    return RETURN_FAIL;
  }
}
