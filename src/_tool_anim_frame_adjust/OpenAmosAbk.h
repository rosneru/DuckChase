#ifndef OPEN_AMOS_ABK_H
#define OPEN_AMOS_ABK_H

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
};

#endif
