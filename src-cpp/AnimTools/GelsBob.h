#ifndef GELS_GelsBob_H
#define GELS_GelsBob_H

#include <intuition/screens.h>
#include "animtools.h"


/**
 * Encapsulation of an Amiga blitter object (Bob) of the AmigaOS GELs
 * system.
 *
 * Currently it is using the animtools structures and functions as
 * presented in the RKRM libraries.
 *
 * @author Uwe Rosner
 * @date 11/07/2019
 */
class GelsBob
{
public:
  GelsBob(struct Screen* p_pScreen, short p_ScreenDepth);
  ~GelsBob();

  bool CreateFromRawFile(const char* p_pPath,
                         int p_Width,
                         int p_Height,
                         short p_Depth);

  bool CreateFromArray(const WORD* p_pAddress,
                       int p_Width,
                       int p_Height,
                       short p_Depth);

  struct Bob* GetBob();

private:
  struct Screen* m_pScreen;
  NEWBOB m_NewBob;
  struct Bob* m_pBob;
  WORD* m_pImageData;

  void clear();
};

#endif
