#ifndef ANIM_SHEET_CONTAINER_H
#define ANIM_SHEET_CONTAINER_H

#include <exec/nodes.h>
#include <exec/lists.h>
#include <graphics/gfx.h>

#include <string>

#include "OpenIlbmPictureBitMap.h"


enum SheetDataType
{
  SDT_None,
  SDT_IlbmPicture,
  SDT_AmosBank,
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
   * it overwrites the current file
   */
  bool save(const char* pFileName = NULL);

  struct SheetItemNode* getSheet(ULONG index);

  struct List* getSheetList();

  ULONG* getColors32();

  ULONG getNumSheets();

  const char* getFileName();
  void setFilename(const char* pFileName);

  bool isIlbmSheet();
  bool isAmosSheet();

private:
  SheetDataType m_SheetDataType;
  struct List m_SheetList;
  ULONG m_NumSheets;
  ULONG* m_pColors32;
  ULONG m_ModeId;

  std::string m_Filename;

  bool addItemNode(const struct BitMap* pBitMap, 
                   ULONG initialIndex, 
                   ULONG wordWidth = 0);

/**
 * Creates a copy of the color table of given BitMapPicture. Allocates
 * memory which after using must be freed with FreeVec(). The color data
 * can be used with LoadRgb32().
 *
 * @returns On success: the address of the the color table copy,
 *          on error: NULL.
 */
  ULONG* deepCopyColors(ULONG* pSrcColors32, ULONG srcDepth);

  void cleanup();
};

#endif
