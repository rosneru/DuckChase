#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <graphics/modeid.h>
#include <graphics/videocontrol.h>

#include "GameViewIntui.h"

extern struct GfxBase* GfxBase;


GameViewIntui::GameViewIntui(short viewWidth,
                               short viewHeight,
                               short viewDepth)
  : m_ViewWidth(viewWidth),
    m_ViewHeight(viewHeight),
    m_ViewDepth(viewDepth),
    m_pScreen(NULL),
    m_pBitMapArray(),
    m_FrameToggle(0),
    m_InitError(IE_None)
{
  m_ViewNumColors = 1L << viewDepth;
}


GameViewIntui::~GameViewIntui()
{

}

bool GameViewIntui::Open()
{
  if(m_pScreen != NULL)
  {
    m_InitError = IE_AlreadyInitialized;
    return false;
  }

  m_InitError = IE_None;


  for(int i = 0; i < 2; i++)
  {
    // Allocate memory for BitMap i
    m_pBitMapArray[i] = (struct BitMap *)
     AllocVec(sizeof(struct BitMap), MEMF_CHIP);

    if(m_pBitMapArray[i] == NULL)
    {
      m_InitError = IE_GettingBitMapMem;
      Close();
      return false;
    }

    // Init BitMap i
    InitBitMap(m_pBitMapArray[i], m_ViewDepth, m_ViewWidth,
               m_ViewHeight);

    // Set the plane pointers to NULL so the cleanup routine will know
    // if they were used
    for (int depth = 0; depth < m_ViewDepth; depth++)
    {
      m_pBitMapArray[i]->Planes[depth] = NULL;
    }

    // Allocate memory for the BitPlanes
    for (int depth = 0; depth < m_ViewDepth; depth++)
    {
      m_pBitMapArray[i]->Planes[depth] = (PLANEPTR)
        AllocRaster(m_ViewWidth, m_ViewHeight);

      if (m_pBitMapArray[i]->Planes[depth] == NULL)
      {
        m_InitError = IE_GettingBitPlanes;
        Close();
        return false;
      }

      // Set all bits of this newly created BitPlane to 0
      BltClear(m_pBitMapArray[i]->Planes[depth],
               (m_ViewWidth / 8) * m_ViewHeight, 1);
    }
  }

  // Additional setting for the screen to use hires sprites
  struct TagItem vcTags[] =
  {
    {VTAG_SPRITERESN_SET, SPRITERESN_70NS},
    {TAG_END}
  };

  m_pScreen = OpenScreenTags(NULL,
    SA_DisplayID, PAL_MONITOR_ID|HIRES_KEY,
    SA_Depth, m_ViewDepth,
    SA_Width, m_ViewWidth,
    SA_Height, m_ViewHeight,
    SA_ShowTitle, FALSE,
    SA_VideoControl, vcTags,
    SA_Quiet, TRUE,
    SA_Type, CUSTOMSCREEN,
    SA_Exclusive, TRUE,
    SA_BitMap, m_pBitMapArray[0],
    TAG_DONE);

  if(m_pScreen == NULL)
  {
    m_InitError = IE_OpeningScreen;
    Close();
    return false;
  }


  return true;
}

void GameViewIntui::Close()
{
  if(m_pScreen != NULL)
  {
    CloseScreen(m_pScreen);
    m_pScreen = NULL;
  }

  //  Free the double buffers
  for(int i = 0; i < 2; i++)
  {
    if(m_pBitMapArray[i] != NULL)
    {
      // Free all BitPlanes of this buffer
      for (int depth = 0; depth < m_ViewDepth; depth++)
      {
        if (m_pBitMapArray[i]->Planes[depth] != NULL)
        {
          FreeRaster(m_pBitMapArray[i]->Planes[depth],
                     m_ViewWidth, m_ViewHeight);

          m_pBitMapArray[i]->Planes[depth] = NULL;
        }
      }

      // Then free the buffer itself
      FreeVec(m_pBitMapArray[i]);
      m_pBitMapArray[i] = NULL;
    }
  }
}

short GameViewIntui::Width()
{
  return m_ViewWidth;
}


short GameViewIntui::Height()
{
  return m_ViewHeight;
}


short GameViewIntui::Depth()
{
  return m_ViewDepth;
}


struct RastPort* GameViewIntui::RastPort()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &(m_pScreen->RastPort);
}


struct ViewPort* GameViewIntui::ViewPort()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &(m_pScreen->ViewPort);
}

void GameViewIntui::SetColor32(int i, int r, int g, int b)
{
  if(m_pScreen != NULL)
  {
    SetRGB32(&m_pScreen->ViewPort, i, r, g, b);
  }
}

void GameViewIntui::Render()
{
  if(m_pScreen == NULL)
  {
    return;
  }

  SortGList(&(m_pScreen->RastPort));
  DrawGList(&(m_pScreen->RastPort), &(m_pScreen->ViewPort));

  // Double buffering: One BitMap is for the view
  m_pScreen->ViewPort.RasInfo->BitMap = m_pBitMapArray[m_FrameToggle];

  WaitTOF();

  MrgCop(ViewAddress());    // TODO Avoid multiple calls
  LoadView(ViewAddress());

  // Double buffering: Toggle the BitMap pointer
  m_FrameToggle ^= 1;

  // Double buffering: Now the other BitMap is for drawing
  m_pScreen->RastPort.BitMap = m_pBitMapArray[m_FrameToggle];

}

const char* GameViewIntui::LastError() const
{
  switch(m_InitError)
  {
    case IE_None:
      return "No error: init done successfully.";
      break;

    case IE_AlreadyInitialized:
      return "GameView already initialized.";
      break;

    case IE_GettingBitMapMem:
      return "Could not get BitMap memory.\n";
      break;

    case IE_GettingBitPlanes:
      return "Could not get BitPlanes.\n";
      break;

    case IE_OpeningScreen:
      return "Could not open the screen.\n";
      break;

    default:
      return "Unknown state";
      break;
  }
}

const char* GameViewIntui::ViewName() const
{
  return "Simple (Intuition) view";
}
