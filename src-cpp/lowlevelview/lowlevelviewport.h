#ifndef LOWLEVELVIEWPORT_H
#define LOWLEVELVIEWPORT_H

#include <graphics/displayinfo.h>
#include <graphics/view.h>


/**
 * Represents a low level viewport object. Adapted from the example
 * LowLevelView.c as described in the Amiga CD32 Developer Manual.
 *
 * @author Uwe Rosner
 * @date 27/08/2019
 */
class LowlevelViewPort
{
public:
  LowlevelViewPort();
  ~LowlevelViewPort();

  bool Create(ULONG sizex, ULONG sizey, ULONG depth, ULONG modeId,
              ULONG colors, struct BitMap* pBitMap);

  void Delete();

  struct ViewPort* ViewPort();

  const char* LastError() const;


private:
  struct ViewPort* m_pViewPort;
  struct ViewPortExtra* m_pViewPortExtra;
  struct DisplayInfo* m_pDisplayInfo;

  enum InitError
  {
    IE_None,
    IE_ColorMapTooSmall,
    IE_GettingViewportMem,
    IE_GettingColorMap,
    IE_GettingRasInfo,
    IE_GettingVPExtra,
    IE_GettingDisplayInfo,
    IE_SettingVideoControl,
    IE_SettingPalExtra,
  };

  InitError m_InitError;

};

#endif