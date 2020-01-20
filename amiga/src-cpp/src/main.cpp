#include <graphics/modeid.h>

#include <stdio.h>

#include "Game.h"
#include "GameViewLowlevel.h"
#include "GameViewIntui.h"
#include "GameViewGfxLib.h"


int main(void)
{
  //GameViewLowlevel gameView(640, 256, 3, 32, PAL_MONITOR_ID|HIRES_KEY);
  GameViewIntui gameView(640, 256, 3);
  //GameViewGfxLib gameView(640, 256, 3);
  Game game(gameView);

  if(game.Run() == false)
  {
    printf("%s", game.LastError());
  }

  return 0;
}
