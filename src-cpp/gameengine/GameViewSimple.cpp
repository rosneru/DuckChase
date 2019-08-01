#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <graphics/gfxbase.h>

#include "GameViewSimple.h"

extern struct GfxBase* GfxBase;


GameViewSimple::GameViewSimple(short viewWidth,
                               short viewHeight,
                               short viewDepth)
  : m_ViewWidth(viewWidth),
    m_ViewHeight(viewHeight),
    m_ViewDepth(viewDepth),
    m_pScreen(NULL),
    m_BufToggle(false)
{
  m_ViewNumColors = 1;
  for(int i = 0; i < viewDepth; i++)
  {
    m_ViewNumColors *= 2;
  }
}


GameViewSimple::~GameViewSimple()
{

}

bool GameViewSimple::Open()
{
  if(m_pScreen != NULL)
  {
    m_InitError = IE_AlreadyInitialized;
    return false;
  }

  m_InitError = IE_None;


  // Initialize the BitMaps
  InitBitMap(&m_BitMap1, m_ViewDepth, m_ViewWidth, m_ViewHeight);
  InitBitMap(&m_BitMap2, m_ViewDepth, m_ViewWidth, m_ViewHeight);

  // Set the plane pointers to NULL so the cleanup routine
  // will know if they were used
  for (int depth = 0; depth < m_ViewDepth; depth++)
  {
    m_BitMap1.Planes[depth] = NULL;
    m_BitMap2.Planes[depth] = NULL;
  }

  // Allocate space for BitMap
  for (int depth = 0; depth < m_ViewDepth; depth++)
  {
    m_BitMap1.Planes[depth] = (PLANEPTR)
      AllocRaster(m_ViewWidth, m_ViewHeight);

    if (m_BitMap1.Planes[depth] == NULL)
    {
      m_InitError = IE_GettingBitPlanes;
      return false;
    }

    m_BitMap2.Planes[depth] = (PLANEPTR)
      AllocRaster(m_ViewWidth, m_ViewHeight);

    if (m_BitMap2.Planes[depth] == NULL)
    {
      m_InitError = IE_GettingBitPlanes;
      return false;
    }
  }

  UWORD pens[] = { ~0 };

  m_pScreen = OpenScreenTags(NULL,
    SA_Pens, pens,
    SA_Depth, m_ViewDepth,
    SA_Top, 0,
    SA_Left, 0,
    SA_Width, m_ViewWidth,
    SA_Height, m_ViewHeight,
    SA_DisplayID, PAL_MONITOR_ID|HIRES_KEY,
    SA_Quiet, TRUE,
    SA_Exclusive, TRUE,
//    SA_Interleaved, TRUE,
    SA_Type, CUSTOMSCREEN,
    SA_BitMap, &m_BitMap1,
    TAG_END);

  if(m_pScreen == NULL)
  {
    return false;
  }


  return true;
}

void GameViewSimple::Close()
{
  if(m_pScreen != NULL)
  {
    CloseScreen(m_pScreen);
  }

  //  Free the BitPlanes drawing area
  for (int depth = 0; depth < m_ViewDepth; depth++)
  {
    if (m_BitMap1.Planes[depth] != NULL)
    {
      FreeRaster(m_BitMap1.Planes[depth], m_ViewWidth, m_ViewHeight);
    }

    if (m_BitMap2.Planes[depth] != NULL)
    {
      FreeRaster(m_BitMap2.Planes[depth], m_ViewWidth, m_ViewHeight);
    }

  }
}

short GameViewSimple::Width()
{
  return m_ViewWidth;
}


short GameViewSimple::Height()
{
  return m_ViewHeight;
}


short GameViewSimple::Depth()
{
  return m_ViewDepth;
}


struct RastPort* GameViewSimple::RastPort()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &m_pScreen->RastPort;
}


struct ViewPort* GameViewSimple::ViewPort()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &m_pScreen->ViewPort;
}

struct View* GameViewSimple::View()
{
  return ViewAddress();
}

void GameViewSimple::Render()
{
  if(m_pScreen == NULL)
  {
    return;
  }

  SortGList(&m_pScreen->RastPort);
  DrawGList(&m_pScreen->RastPort, &m_pScreen->ViewPort);
  WaitTOF();

  MrgCop(ViewAddress());    // TODO Avoid multiple calls
  LoadView(ViewAddress());

  // Switch the buffers
  if(m_BufToggle == false)
  {
    m_pScreen->ViewPort.RasInfo->BitMap = &m_BitMap2;
    m_pScreen->RastPort.BitMap = &m_BitMap2;
    m_BufToggle = true;
  }
  else
  {
    m_pScreen->ViewPort.RasInfo->BitMap = &m_BitMap1;
    m_pScreen->RastPort.BitMap = &m_BitMap1;
    m_BufToggle = false;
  }
}

const char* GameViewSimple::LastError() const
{
  switch(m_InitError)
  {
    case IE_None:
      return "No error: init done successfully.";
      break;

    case IE_AlreadyInitialized:
      return "GameView already initialized.";
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

const char* GameViewSimple::ViewName() const
{
  return "Simple (Intuition) view";
}
