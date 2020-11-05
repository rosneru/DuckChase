#ifndef SAVE_AMOS_ABK_H
#define SAVE_AMOS_ABK_H

#include <dos/dos.h>
#include <exec/types.h>
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
  /**
   * Creating the saver object already saves the file.
   */
  SaveAmosAbk(const char* pFileName,
              std::vector<SheetItemNode*> sheets,
              ULONG* pColors32);

  virtual ~SaveAmosAbk();

private:
  BPTR m_FileHandle;
  ULONG* m_pOCSColorTable;

  
  void cleanup();
  
  ULONG* colors32ToOCSColorTable(ULONG* pColors32);
  
  bool writeWord(ULONG value);

};

#endif
