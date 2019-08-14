#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <graphics/modeid.h>

#include "GameViewIntui30.h"

extern struct GfxBase* GfxBase;


GameViewIntui30::GameViewIntui30(short viewWidth,
                                 short viewHeight,
                                 short viewDepth)
  : m_ViewWidth(viewWidth),
    m_ViewHeight(viewHeight),
    m_ViewDepth(viewDepth),
    m_pScreen(NULL),
    m_pBitMapArray(),
    m_FrameToggle(0),
    m_pDBufInfo(NULL),
    m_pMsgPortArray(),
    m_InitError(IE_None)
{
  m_ViewNumColors = 1;
  for(int i = 0; i < viewDepth; i++)
  {
    m_ViewNumColors *= 2;
  }
}


GameViewIntui30::~GameViewIntui30()
{

}

bool GameViewIntui30::Open()
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

  m_pScreen = OpenScreenTags(NULL,
    SA_Depth, m_ViewDepth,
    SA_Top, 0,
    SA_Left, 0,
    SA_Width, m_ViewWidth,
    SA_Height, m_ViewHeight,
    SA_DisplayID, PAL_MONITOR_ID|HIRES_KEY,
    SA_Quiet, TRUE,
    SA_Exclusive, TRUE,
    SA_BitMap, m_pBitMapArray[0],
    TAG_END);

  if(m_pScreen == NULL)
  {
    m_InitError = IE_OpeningScreen;
    Close();
    return false;
  }

  m_pDBufInfo = AllocDBufInfo(&(m_pScreen->ViewPort));
  if(m_pDBufInfo == NULL)
  {
    m_InitError = IE_GettingDBufInfo;
    return false;
  }

  m_pMsgPortArray[0] = CreateMsgPort();
  if(m_pMsgPortArray[0] == NULL)
  {
    m_InitError = IE_CreatingMsgPort1;
  }

  m_pMsgPortArray[1] = CreateMsgPort();
  if(m_pMsgPortArray[1] == NULL)
  {
    m_InitError = IE_CreatingMsgPort2;
  }

  return true;
}

void GameViewIntui30::Close()
{
  if(m_pMsgPortArray[1] != NULL)
  {
    DeleteMsgPort(m_pMsgPortArray[1]);
    m_pMsgPortArray[1] = NULL;
  }

  if(m_pMsgPortArray[0] != NULL)
  {
    DeleteMsgPort(m_pMsgPortArray[0]);
    m_pMsgPortArray[0] = NULL;
  }

  if(m_pDBufInfo != NULL)
  {
    FreeDBufInfo(m_pDBufInfo);
    m_pDBufInfo = NULL;
  }

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

short GameViewIntui30::Width()
{
  return m_ViewWidth;
}


short GameViewIntui30::Height()
{
  return m_ViewHeight;
}


short GameViewIntui30::Depth()
{
  return m_ViewDepth;
}


struct RastPort* GameViewIntui30::RastPort()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &(m_pScreen->RastPort);
}


struct ViewPort* GameViewIntui30::ViewPort()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &(m_pScreen->ViewPort);
}


void GameViewIntui30::Render()
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

const char* GameViewIntui30::LastError() const
{
  switch(m_InitError)
  {
    case IE_None:
      return "No error: init done successfully.";
      break;

    case IE_AlreadyInitialized:
      return "GameView already initialized.";
      break;

    case IE_CreatingMsgPort1:
      return "Could not get MsgPort 1 for double buffering.\n";
      break;

    case IE_CreatingMsgPort2:
      return "Could not get MsgPort 2 for double buffering.\n";
      break;

    case IE_GettingDBufInfo:
      return "Could not get DBufInfo.\n";
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

const char* GameViewIntui30::ViewName() const
{
  return "Simple (Intuition) view";
}
