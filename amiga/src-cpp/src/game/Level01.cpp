#include <clib/lowlevel_protos.h>
#include <libraries/lowlevel.h>

#include "IlbmBitmap.h"
#include "Level01.h"


Level01::Level01(GameViewBase& gameView, 
                 InfoDisplay& infoDisplay,
                 GameVars& gameVars,
                 const GameWorld& gameWorld,
                 StopWatch& stopWatch)
  : m_GameView(gameView),
    m_InfoDisplay(infoDisplay),
    m_GameVars(gameVars),
    m_StopWatch(stopWatch),
    m_IsArrowFlightPrepared(false),
    m_IsArrowFlightFinished(false),
    m_IsStrike(false),
    m_LastFps(16),
    m_Strain(0),
    m_StrainMustUpdateSecondBuffer(false),
    m_ArrowsMustUpdateSecondBuffer(false),
    m_Duck(m_GameView, gameWorld, m_DuckRes),
    m_Arrow(m_GameView, 
            gameWorld, 
            m_ArrowRes, 
            m_Duck, 
            m_Strain, 
            m_IsArrowFlightFinished, 
            m_IsStrike),
    m_Hunter(gameView, 
              gameWorld,
              m_HunterRes,
              m_Arrow,
              m_IsArrowFlightPrepared)
{
  m_Arrow.Deactivate();

  // Initialize the info display with the available arrows
  m_InfoDisplay.UpdateArrows();

  // Do it also in the 2nd screen buffer
  m_GameView.Render();
  m_InfoDisplay.UpdateArrows();

  // *Must* be done after all sprites are allocated. Intuition then
  // re-adjustes everything (maybe also reduces gfx bandwitht) to ensure
  // that the sprites can be displayed.
  RemakeDisplay();
}


Level01::~Level01()
{

}

void Level01::DisableDoubleBuf()
{

}


void Level01::Run()
{
  m_StopWatch.Start();



  gameLoop();
  displayWinner();

  // Remove all shapes
  m_Arrow.Deactivate();
  m_Hunter.Deactivate();
  m_Duck.Deactivate();

  // Render twice to reflect this in double-buffer
  m_GameView.Render();
  m_GameView.Render();
}

void Level01::gameLoop()
{
  short frameCounter = -2;
  long elapsed_ms = 0;
  long elapsedSinceLastBarrel_ms = 40;
  bool justDrawFps = false;
  bool bContinue = true;
  do
  {
    //
    // Calculate the elapsed time and update the FPS value
    //
    frameCounter++;
    if(frameCounter > 16)
    {
      frameCounter = 1;
      elapsed_ms = m_StopWatch.Pick(false) >> 4;
      elapsedSinceLastBarrel_ms += elapsed_ms;
      if(elapsed_ms > 0)
      {
        // Sum the fps of each frame
        m_LastFps = (short)(1000 / elapsed_ms);
        m_InfoDisplay.UpdateFps(m_LastFps);

        // To also appear in the double buffer, ensure that fps is
        // updated in next frame too.
        justDrawFps = true;
      }
    }
    else if(justDrawFps)
    {
      m_InfoDisplay.UpdateFps(m_LastFps);
      justDrawFps = false;
    }

    //
    // Read the Joyport and update the entities
    //
    ULONG portState = ReadJoyPort(1);

    m_Duck.Update(elapsed_ms, portState);
    m_Arrow.Update(elapsed_ms, portState);
    m_Hunter.Update(elapsed_ms, portState);

    //
    // Check if hunter is currently arming an arrow and update the info
    // display accordingly
    //
    if(m_IsArrowFlightPrepared)
    {
      m_Strain += 4;
      m_InfoDisplay.UpdateStrain(m_Strain, false);

      // Strain must be updated in 2nd screen buffer after switched
      m_StrainMustUpdateSecondBuffer = true;
    }
    else if(m_IsArrowFlightFinished)
    {
      m_GameVars.DecreaseNumArrows();
      m_InfoDisplay.UpdateArrows();

      // Arrows must be updated in 2nd screen buffer after switched
      m_ArrowsMustUpdateSecondBuffer = true;

      m_IsArrowFlightFinished = false;
    }
    else if(m_Arrow.IsAlive())
    {
      m_Strain = 0;
      m_InfoDisplay.UpdateStrain(m_Strain, false);

      // Strain must be updated in 2nd screen buffer after switched
      m_StrainMustUpdateSecondBuffer = true;      
    }
    

    //
    // Render the entities and switch screen buffers
    //
    m_GameView.Render();

    //
    // Update changed strain and/or arrows on InfoDisplay for 2nd screen
    // buffer too (if necessary)
    //
    if(m_StrainMustUpdateSecondBuffer)
    {
      m_InfoDisplay.UpdateStrain(m_Strain, true);
      m_StrainMustUpdateSecondBuffer = false;
    }

    if(m_ArrowsMustUpdateSecondBuffer)
    {
      m_InfoDisplay.UpdateArrows();
      m_ArrowsMustUpdateSecondBuffer = false;
    }

    //
    // Check if exit key ESC has been pressed
    //
    ULONG key = GetKey();
    if((key & 0xff) == 0x45)
    {
      // ESC pressed, quit the game
      bContinue = false;
    }

    if(m_GameVars.NumArrows() == 0 && !m_Arrow.IsAlive())
    {
      bContinue = false;
    }

    if(m_IsStrike)
    {
      bContinue = false;
    }
  }
  while (bContinue);
}

void Level01::displayWinner()
{
  char const* pWinnerDuckImagePath = "AADevDuck:assets/winner_duck_comic.ilbm";
  char const* pWinnerHunterImagePath = "AADevDuck:assets/winner_hunter_comic.ilbm";

  const char* pImagePath;
  if(m_IsStrike)
  {
    pImagePath = pWinnerHunterImagePath;
  }
  else
  {
    pImagePath = pWinnerDuckImagePath;
  }
  
  IlbmBitmap winnerPicture(pImagePath, false, false);
  const struct BitMap* pMask = winnerPicture.CreateBitMapMask();
  if(pMask == NULL)
  {
    return;
  }

  // Blit the selected 'duck wins' image on the screen
  int x = (m_GameView.Width() - winnerPicture.Width()) / 2;
  int y = (m_GameView.Height() - winnerPicture.Height()) / 2;

  // Blit the winner pic into both screen buffers
  for(size_t i = 0; i < 2; i++)
  {
    BltMaskBitMapRastPort(winnerPicture.GetBitMap(),
                          0,
                          0,
                          m_GameView.RastPort(),
                          x,
                          y,
                          winnerPicture.Width(),
                          winnerPicture.Height(),
                          0xe0,
                          pMask->Planes[0]);

    WaitBlit();
    m_GameView.Render();
  }
}
