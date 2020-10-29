#ifndef ANIM_SHEET_CONTAINER_H
#define ANIM_SHEET_CONTAINER_H

#include <exec/nodes.h>
#include <exec/lists.h>
#include <graphics/gfx.h>

#include "OpenIlbmPictureBitMap.h"


enum SheetDataType
{
  SDT_None,
  SDT_IlbmPicture,
  SDT_AmosBank,
};

/**
 * A defaukt exec list, extended by the data needed for anim sheets
 */
struct SheetItemNode
{
  struct Node ld_Node;
  void* pSheetContainer;
};


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
class AnimSheetContainer
{
public:
  AnimSheetContainer(const char* pFileName);
  virtual ~AnimSheetContainer();

  /**
   * Save the current animation to the given name. If no name is given
   * it creates one.
   *
   * Throws an exception on error.
   */
  void save(const char* pFileName = NULL);

  OpenIlbmPictureBitMap* getCurrent();

  struct List* getSheetList();

private:
  SheetDataType m_SheetDataType;
  struct List m_SheetList;

  bool addItemNode(BitMapPictureBase* pPic, size_t initialIndex);
  void cleanup();
};

#endif
