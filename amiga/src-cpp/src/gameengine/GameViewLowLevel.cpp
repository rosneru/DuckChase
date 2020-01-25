#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <graphics/copper.h>
#include <graphics/gfxbase.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxnodes.h>
#include <graphics/videocontrol.h>
#include <intuition/screens.h>

#include "GameViewLowlevel.h"

extern struct GfxBase* GfxBase;


GameViewLowlevel::GameViewLowlevel(short width,
                                   short height,
                                   short depth,
                                   short numColors,
                                   ULONG modeId)
  : m_Width(width),
    m_Height(height),
    m_Depth(depth),
    m_NumColors(numColors),
    m_ModeId(modeId),
    m_pLastError("Initialized sucessfully."),
    m_pOldView(NULL),
    m_pView(NULL),
    m_pViewPort(NULL),
    m_RastPort(),
    m_FrameToggle(0),
    m_pBitMapArray()
{

}


GameViewLowlevel::~GameViewLowlevel()
{

}

bool GameViewLowlevel::Open(GameColors& colors)
{
  if(m_pViewPort != NULL)
  {
    Close();
    return false;
  }

  // Initialize the BitMaps
  for(int i = 0; i < 2; i++)
  {
    // Allocate memory for BitMap i
    m_pBitMapArray[i] = (struct BitMap *)
     AllocVec(sizeof(struct BitMap), MEMF_CHIP);

    if(m_pBitMapArray[i] == NULL)
    {
      m_pLastError = "Can't allocate BitMap memory.\n";
      Close();
      return false;
    }

    // Init BitMap i
    InitBitMap(m_pBitMapArray[i], m_Depth, m_Width, m_Height);

    // Set the plane pointers to NULL so the cleanup routine will know
    // if they were used
    for (int depth = 0; depth < m_Depth; depth++)
    {
      m_pBitMapArray[i]->Planes[depth] = NULL;
    }

    // Allocate memory for the BitPlanes
    for (int depth = 0; depth < m_Depth; depth++)
    {
      m_pBitMapArray[i]->Planes[depth] = (PLANEPTR)
        AllocRaster(m_Width, m_Height);

      if (m_pBitMapArray[i]->Planes[depth] == NULL)
      {
        m_pLastError = "Can't get BitPlanes.\n";
        return false;
        Close();
      }

      // Set all bits of this newly created BitPlane to 0
      BltClear(m_pBitMapArray[i]->Planes[depth],
               (m_Width / 8) * m_Height, 1);
    }
  }

  // This screen is only a trick: It just exists to ensure that after
  // closing the main view *Intuition* handles important things like
  // giving back the mouse pointer etc. which were lost by creating
  // our own view. So it is only opened with a depth of 1 losing about
  // 20k in HiRes or 10 in LoRes.
  m_pDummyScreen = OpenScreenTags(NULL,
      SA_DisplayID, m_ModeId,
      SA_Depth, 1,
      SA_Width, m_Width,
      SA_Height, m_Height,
      SA_ShowTitle, FALSE,
      TAG_DONE);

  if (m_pDummyScreen == NULL)
  {
    m_pLastError = "Faild to open the dummy screen!\n";
    Close();
    return false;
  }

  if(m_LowLevelView.Create(m_ModeId) == false)
  {
    m_pLastError = (char*)m_LowLevelView.LastError();
    Close();
    return false;
  }

  m_pView = m_LowLevelView.View();

  if(m_LowLevelViewPort.Create(m_Width,
                               m_Height,
                               m_Depth,
                               m_ModeId,
                               m_NumColors,
                               m_pBitMapArray[0],
                               colors.GetViewColorsLoadRGB32()) == false)
  {
    m_pLastError = (char*)m_LowLevelViewPort.LastError();
    Close();
    return false;
  }

  m_pViewPort = m_LowLevelViewPort.ViewPort();


  // Create a RastPort to draw into
  InitRastPort(&m_RastPort);
  m_RastPort.BitMap = m_pViewPort->RasInfo->BitMap;
  SetRast(&m_RastPort, 0);

  m_pView->ViewPort = m_pViewPort;

  // Construct preliminary Copper instruction list
  MakeVPort(m_pView, m_pViewPort);

  // Merge preliminary lists into a real Copper list in the View
  // structure
  MrgCop(m_pView);

  //Save current View to restore later
  m_pOldView = GfxBase->ActiView;

  do
  {
    LoadView(NULL);
    WaitTOF();
    WaitTOF();
  }
  while(GfxBase->ActiView != NULL);

  LoadView(m_pView);
  WaitTOF();
  WaitTOF();

  return true;
}

void GameViewLowlevel::Close()
{
  if(m_pView != NULL)
  {
    if (GfxBase->ActiView == m_pView)
    {
      do
      {
        LoadView(NULL);
        WaitTOF();
        WaitTOF();
      }
      while(GfxBase->ActiView != NULL);

      // Put back the old view
      LoadView(m_pOldView);

      // Before freeing memory wait until the old view is being rendered
      WaitTOF();
      WaitTOF();
    }
  }

  if(m_pViewPort != NULL)
  {
    m_LowLevelViewPort.Delete();
    m_pViewPort = NULL;
  }
  if(m_pView != NULL)
  {
    m_LowLevelView.Delete();
    m_pView = NULL;
  }

  if(m_pDummyScreen != NULL)
  {
    CloseScreen(m_pDummyScreen);
    m_pDummyScreen = NULL;
  }

  //  Free the double buffers
  for(int i = 0; i < 2; i++)
  {
    if(m_pBitMapArray[i] != NULL)
    {
      // Free all BitPlanes of this buffer
      for (int depth = 0; depth < m_Depth; depth++)
      {
        if (m_pBitMapArray[i]->Planes[depth] != NULL)
        {
          FreeRaster(m_pBitMapArray[i]->Planes[depth],
                     m_Width, m_Height);

          m_pBitMapArray[i]->Planes[depth] = NULL;
        }
      }

      // Then free the buffer itself
      FreeVec(m_pBitMapArray[i]);
      m_pBitMapArray[i] = NULL;
    }
  }
}


short GameViewLowlevel::Width()
{
  return m_Width;
}


short GameViewLowlevel::Height()
{
  return m_Height;
}


short GameViewLowlevel::Depth()
{
  return m_Depth;
}


struct RastPort* GameViewLowlevel::RastPort()
{
  return &m_RastPort;
}


struct ViewPort* GameViewLowlevel::ViewPort()
{
  return m_pViewPort;
}


void GameViewLowlevel::Render()
{
  if(GfxBase->ActiView != m_pView)
  {
    // Not initialized
    return;
  }

  SortGList(&m_RastPort);
  DrawGList(&m_RastPort, m_pViewPort);

  // Double buffering: One BitMap is for the view
  m_pViewPort->RasInfo->BitMap = m_pBitMapArray[m_FrameToggle];

  WaitTOF();

  MrgCop(m_pView);
  LoadView(m_pView);

  // Double buffering: Toggle the BitMap pointer
  m_FrameToggle ^= 1;

  // Double buffering: Now the other BitMap is for drawing
  m_RastPort.BitMap = m_pBitMapArray[m_FrameToggle];
}

const char* GameViewLowlevel::LastError() const
{
  return m_pLastError;
}

const char* GameViewLowlevel::ViewName() const
{
  return "Lowlevel view";
}
