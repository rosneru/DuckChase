#include "GameView.h"


GameView::GameView(short viewWidth, short viewHeight, short viewDepth)
  : m_ViewWidth(viewWidth),
    m_ViewHeight(viewHeight),
    m_ViewDepth(viewDepth),
    //viewPort(0),
    //bitMap1(0),
    //bitMap2(0),
    pBitMap(NULL),
    cm(NULL),
    vextra(NULL),
    monspec(NULL),
    vpextra(NULL)
    //dimquery(0)
{
}


GameView::~GameView()
{

}

bool GameView::Init()
{
  return false;
}
