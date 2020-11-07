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
  
  /**
   * Create a planar image data in a way that all BitPlanes are locate
   * one after another.
   *
   * The full size of the allocated Imagedata will be written to the
   * provided byteSize variable.
   *
   * IMPORTANT: Must be freed with FreeVec() after use.
   */
  WORD* sheetBitmapToFrameImage(struct BitMap* pSrcBitmap, 
                          ULONG xStart,
                          ULONG sheetNumFrames,
                          ULONG& bufSizeBytes);

  ULONG* colors32ToOCSColorTable(ULONG* pColors32);
  
  void writeWord(ULONG value);

};

#endif
