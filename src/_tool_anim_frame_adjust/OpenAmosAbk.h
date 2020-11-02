#ifndef OPEN_AMOS_ABK_H
#define OPEN_AMOS_ABK_H

#include <vector>

#include <dos/dos.h>
#include <exec/types.h>

class OpenAmosAbk
{
public:
  OpenAmosAbk(const char* pFileName);
  virtual ~OpenAmosAbk();

  /**
   * Parses the next AnimSheet and returns its BitMap.
   *
   * An AnimSheet consits of a number of succeeding pictures with equal
   * width and height.
   */
  struct BitMap* parseNextAnimSheet();

  /**
   * Parses the colors of this AMOS bank, convert it to Colors32 format
   * and return it.
   */
  ULONG* parseColors32();

private:
  BYTE* m_pFileBuf;
  BPTR m_FileHandle;
  ULONG* m_pColors32;
  ULONG m_FileBufByteSize;
  ULONG m_ParseByteCounter;
  
  ULONG m_NumAbkFrames;
  ULONG m_AbkFrameId;

  ULONG m_SheetFramesWordWidth;
  ULONG m_SheetFramesHeight;
  ULONG m_SheetFramesDepth;

  struct BitMap* m_pSheetBitMap;

  ULONG readNextWord();

  struct BitMap* createFrameBitMap();
  bool createSheetBitMap(std::vector<struct BitMap*>& frameVec);

  void clearBitMapVector(std::vector<struct BitMap*>& v);

  void cleanup();
};

#endif
