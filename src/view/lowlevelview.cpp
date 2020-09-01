#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#include "lowlevelview.h"

LowlevelView::LowlevelView(ULONG modeId)
  : m_pView(NULL)
{
  m_pView = (struct View*) AllocVec(sizeof(struct View), MEMF_CLEAR);
  if(m_pView == NULL)
  {
    throw "LowlevelView: Failed to allocate memory for View.";
  }

  InitView(m_pView);
}

LowlevelView::~LowlevelView()
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
