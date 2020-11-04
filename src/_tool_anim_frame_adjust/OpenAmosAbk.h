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
   * NOTE: This BitMap must be freed with FreeBitMap() finished using it.
   *
   * An AnimSheet consits of a number of succeeding pictures with equal
   * width and height.
   */
  struct BitMap* parseNextAnimSheet();

  /**
   * Returns the word width of the last parsed anim sheet.
   */
  ULONG getSheetWordWidth();

  /**
   * Parses the colors of this AMOS bank, convert it to Colors32 format
   * and return it.
   *
   * IMPORTANT: Only call this onetime and directly after
   * parseNextAnimSheet() returned NULL.
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

  ULONG m_LastParsedWordWidth;

  struct BitMap* m_pSheetBitMap;

  ULONG readNextWord();

  struct BitMap* createFrameBitMap();

  bool createSheetBitMap(std::vector<struct BitMap*>& frameVec);

  void clearBitMapVector(std::vector<struct BitMap*>& v);

  void cleanup();
};

#endif
