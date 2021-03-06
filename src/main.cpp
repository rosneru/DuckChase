#include <dos/dos.h>
#include <graphics/modeid.h>

#include <stdio.h>


#include "Game.h"
#include "GameViewLowLevel.h"
#include "GameViewIntui.h"
#include "GameWorld.h"
#include "GelsLayer.h"

bool askContinue();

int main(void)
{
  try
  {
    // Load the background image
    OpenIlbmPictureBitMap paletteImg("AADevDuck:assets/gfx/blank_fullsize_Screen.ilbm", 
                          true, 
                          false);

    // Create and open a game view
    // GameViewLowlevel gameView(backgroundPicture, PAL_MONITOR_ID|LORES_KEY);
    GameViewIntui gameView(paletteImg);

    // In this game the playfield does not fill the whole screen
    gameView.SetPlayfieldBorders(0, 40, 255, 280);

    // Create the Gels layer for moving objects (Bobs, Sprites)
    GelsLayer gelsLayer(gameView.RastPort(), 0xff);

    // Create the GameWorld for informations about the tiles, platforms,
    // etc
    GameWorld gameWorld;

    // Create and run the game
    Game game(gameView, gameWorld);
    game.Run();

    return RETURN_OK;
  }
  catch(const char* pMsg)
  {
    printf("%s\n", pMsg);
    return RETURN_FAIL;
  }
}
