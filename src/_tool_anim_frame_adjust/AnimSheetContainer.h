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
  struct BitMap* pBitMap;
  ULONG Width;
  ULONG Height;
  ULONG Depth;
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

  struct SheetItemNode* getSheetItem(ULONG index);

  struct List* getSheetList();

  ULONG* getColors32();

private:
  SheetDataType m_SheetDataType;
  struct List m_SheetList;
  ULONG m_SheetListSize;
  ULONG* m_pColors32;

  bool addItemNode(const BitMapPictureBase& pic, ULONG initialIndex);

/**
 * Creates a copy of the color table of given BitMapPicture. Allocates
 * memory which after using must be freed with FreeVec(). The color data
 * can be used with LoadRgb32().
 *
 * @returns On success: the address of the the color table copy,
 *          on error: NULL.
 */
  ULONG* deepCopyColors(const BitMapPictureBase& pic);

  void cleanup();
};

#endif
