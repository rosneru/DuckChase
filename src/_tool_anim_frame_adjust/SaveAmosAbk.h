#ifndef SAVE_AMOS_ABK_H
#define SAVE_AMOS_ABK_H

#include <graphics/gfx.h>
#include <vector>

#include "SheetItemNode.h"

/**
 * Class for saving a list / vector of BitMaps to an AMOS .abk sprite
 * bank.
 *
 * @author Uwe Rosner
 * @date 05/11/2020
 */
class SaveAmosAbk
{
public:
  SaveAmosAbk(const char* pFileName,
              std::vector<SheetItemNode*> sheets,
              ULONG* pColors32);

  virtual ~SaveAmosAbk();

private:
  void cleanup();
};

#endif
