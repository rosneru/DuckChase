#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include "lowlevelview.h"

LowlevelView::LowlevelView()
  : m_pView(NULL),
    m_pViewExtra(NULL)
{

}

LowlevelView::~LowlevelView()
{
  Delete();
}

bool LowlevelView::Create(ULONG modeId)
{
  if(m_pView != NULL)
  {
    return true;
  }

  m_pView = (struct View*)AllocVec(sizeof(struct View), MEMF_CLEAR);
  if(m_pView == NULL)
  {
    // Can't alocate memory for View
    m_InitError = IE_GettingViewMem;
    Delete();
    return false;
  }

  InitView(m_pView);

  m_pViewExtra = (struct ViewExtra*) GfxNew(VIEW_EXTRA_TYPE);
  if(m_pViewExtra == NULL)
  {
    // Can't get view extra struct
    m_InitError = IE_GettingViewExtra;
    Delete();
    return false;
  }

  m_pViewExtra->Monitor = OpenMonitor(NULL, modeId);
  if(m_pViewExtra->Monitor == NULL)
  {
    // Can't open monitor
    m_InitError = IE_OpeningMonitor;
    Delete();
    return false;
  }

  GfxAssociate(m_pView, m_pViewExtra);

  return true;
}

void LowlevelView::Delete()
{
  if(m_pViewExtra != NULL)
  {
    if(m_pViewExtra->Monitor != NULL)
    {
      CloseMonitor(m_pViewExtra->Monitor);
      m_pViewExtra->Monitor = NULL;

      GfxFree(m_pViewExtra);
      m_pViewExtra = NULL;
    }
  }

  if(m_pView != NULL)
  {
    FreeVec(m_pView);
    m_pView = NULL;
  }
}


struct View* LowlevelView::View()
{
  return m_pView;
}


const char* LowlevelView::LastError() const
{
  switch(m_InitError)
  {
    case IE_None:
      return "No error: init done successfully.\n";
      break;

    case IE_GettingViewMem:
      return "Can't allocate View memory.";
      break;

    case IE_GettingViewExtra:
      return "Can't get ViewExtra.\n";
      break;

    case IE_OpeningMonitor:
      return "Can't open the monitor.\n";
      break;

    default:
      return "Unknown error in LowlevelView.\n";
      break;
  }
}
