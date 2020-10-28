#ifndef ANIM_SHEET_CONTAINER_H
#define ANIM_SHEET_CONTAINER_H

#include "OpenIlbmPictureBitMap.h"

/**
 * A container to load, change and save collection of
 * OpenIlbmPictureBitMap's.
 *
 * If an IFF ILBM file is loaded the collection only contains one sheet.
 *
 * If an AMOS .abk file is loaded, the collection can create more than
 * one sheet.
 *
 * @author Uwe Rosner
 * @date 28/10/2020
 */
class OpenAnimSheets
{
public:
  OpenAnimSheets(const char* pFileName);
  virtual ~OpenAnimSheets();

  /**
   * Save the current animation to the given name. If no name is given
   * it creates one.
   *
   * Throws an exception on error.
   */
  void save(const char* pFileName = NULL);

  OpenIlbmPictureBitMap* getCurrent();

private:
  OpenIlbmPictureBitMap* m_pPicture;

  void cleanup();
};

#endif
