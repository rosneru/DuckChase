#include <libraries/lowlevel.h>
#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>
#include <clib/lowlevel_protos.h>

#include <stdio.h>

#include "Game.h"
#include "GameViewIntui20.h"


int cleanup(int);
int fail(STRPTR);


int main(void)
{
  SetJoyPortAttrs(1,
                  SJA_Type, SJA_TYPE_AUTOSENSE,
                  TAG_END);

  SystemControl(SCON_TakeOverSys, TRUE,
                TAG_END);


  GameViewIntui20 gameView(640, 256, 3);
  Game game(gameView);

  if(game.Run() == false)
  {
    printf("%s", game.LastError());
  }

  SystemControl(SCON_TakeOverSys, FALSE,
                TAG_END);

  return 0;
}
