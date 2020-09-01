/* animtools_proto.h */
#ifndef GELS_LAYER_H
#define GELS_LAYER_H


#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>


/**
 * This class covers a collection of tools which are used with the
 * VSprite, Bob and Animation system software. It is intended as a
 * useful EXAMPLE to the RKRMs, and while it shows what must be done, it
 * is not the only way to do it. If Not Enough Memory, or error return,
 * each cleans up after itself before returning. NOTE that these
 * routines assume a very specific structure to the GEL lists.  Make
 * sure that you use the correct pairs together (i.e. makeOb()/freeOb(),
 * etc.)
 */
class GelsLayer
{
public:
  /**
   * Setup the GELs system.  After this call is made you can use
   * VSprites, Bobs, AnimComps and AnimObs. Note that this links the
   * GelsInfo structure into the RastPort, and calls InitGels(). It uses
   * information in your RastPort structure to establish boundary
   * collision defaults at the outer edges of the raster. This routine
   * sets up for everything - collision detection and all. You must
   * already have run LoadView before ReadyGelSys is called.
   */
  GelsLayer(struct RastPort* pRastPort, BYTE reserved);
  virtual ~GelsLayer();

private:
  struct RastPort *m_pRastPort;
  struct GelsInfo* m_pGelsInfo;
};


#endif
