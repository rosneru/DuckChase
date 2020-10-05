#ifndef GAME_VIEW_LOWLEVEL_H
#define GAME_VIEW_LOWLEVEL_H

#include <graphics/rastport.h>
#include <graphics/view.h>

#include "lowlevelview.h"
#include "lowlevelviewport.h"
#include "OpenIlbmPictureBitMap.h"
#include "GameViewBase.h"


class GameViewDBuf : public GameViewBase
{
public:
  GameViewDBuf(OpenIlbmPictureBitMap& backgroundPicture);
  virtual ~GameViewDBuf();

  virtual struct RastPort* RastPort();
  virtual struct ViewPort* ViewPort();

  void Render();

  const char* ViewName() const;


private:
  struct Screen* m_pScreen;
  struct BitMap* m_pEmptyBitmap;
  void* m_pEmptyPointer;
  struct Window* m_pWindow;
  struct ScreenBuffer* m_ppScreenBuf[2];

  void cleanup();
};

#endif
