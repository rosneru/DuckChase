#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include "lowlevelview.h"

LowlevelView::LowlevelView()
  : m_pView(NULL)
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

  m_pView = (struct View*) AllocVec(sizeof(struct View), MEMF_CLEAR);
  if(m_pView == NULL)
  {
    // Can't alocate memory for View
    m_InitError = IE_GettingViewMem;
    Delete();
    return false;
  }

  InitView(m_pView);
  return true;
}

void LowlevelView::Delete()
{
  if(m_pView != NULL)
  {
    if(m_pView->LOFCprList != NULL)
    {
      // Deallocate the hardware Copper list created by MrgCop()
      FreeCprList(m_pView->LOFCprList);
      m_pView->LOFCprList = NULL;
    }

    if(m_pView->SHFCprList != NULL)
    {
      // Deallocate also the interlace-only hardware Copper list
      FreeCprList(m_pView->SHFCprList);
      m_pView->SHFCprList = NULL;
    }

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
