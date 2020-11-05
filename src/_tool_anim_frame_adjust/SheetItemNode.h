#ifndef SHEET_ITEM_NODE_H
#define SHEET_ITEM_NODE_H

#include <graphics/gfx.h>
#include <exec/nodes.h>
#include <exec/types.h>

/**
 * A default exec list node, extended by the data needed for an
 * AnimSheet
 */
struct SheetItemNode
{
  struct Node ld_Node;
  struct BitMap* pBitMap;
  ULONG FrameWordWidth;
  ULONG SheetWidth;
  ULONG SheetHeight;
  ULONG SheetDepth;
};

#endif
