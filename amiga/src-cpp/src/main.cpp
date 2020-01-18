#include <clib/exec_protos.h>
#include <clib/lowlevel_protos.h>

#include <graphics/modeid.h>
#include <libraries/lowlevel.h>

#include <stdio.h>

#include "Game.h"
#include "GameViewLowlevel.h"
#include "GameViewIntui20.h"
#include "GameViewGfxLib.h"


int main(void)
{
  SetJoyPortAttrs(1,
                  SJA_Type, SJA_TYPE_AUTOSENSE,
                  TAG_END);

  SystemControl(SCON_TakeOverSys, TRUE,
                TAG_END);


  GameViewLowlevel gameView(640, 256, 3, 32, PAL_MONITOR_ID | HIRES_KEY);
  //GameViewIntui20 gameView(640, 256, 3);
  //GameViewGfxLib gameView(640, 256, 3);
  Game game(gameView);

  if(game.Run() == false)
  {
    printf("%s", game.LastError());
  }

  SystemControl(SCON_TakeOverSys, FALSE,
                TAG_END);

  return 0;
}
