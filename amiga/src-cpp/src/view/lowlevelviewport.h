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
  LowlevelViewPort(ULONG sizex, 
                   ULONG sizey, 
                   ULONG depth, 
                   ULONG modeId,
                   ULONG colors, 
                   struct BitMap* pBitMap,
                   const ULONG* pColorArray = NULL);

  virtual ~LowlevelViewPort();

  struct ViewPort* ViewPort();



private:
  struct ViewPort* m_pViewPort;
  struct ViewPortExtra* m_pViewPortExtra;
};

#endif
