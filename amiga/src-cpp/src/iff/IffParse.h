#ifndef IFF_PARSER_H
#define IFF_PARSER_H

#include <libraries/iffparse.h>

/**
 * Creates and holds an IffHandle which is needed by other objects to 
 * parse iff files with the iffparse.library. Also exposes some useful 
 * functions which are helpful e.g. when parsing the BODY chunk.
 * 
 * Code of newiff from Commodore is heavily used.
 *
 * @author Uwe Rosner
 * @date 24/06/2020
 */
class IffParse
{
public:
  IffParse(const char* pFileName);
  virtual ~IffParse();

  struct IFFHandle* Handle();


  /**
   * Returns the ID of the current chunk (e.g. ID_CAMG)
   */
  LONG currentChunkIs(LONG type, LONG id) const;

  /**
   * Computes the worst case packed size of a "row" of bytes.
   */
  inline ULONG maxPackedSize(ULONG rowSize) const
  {
    return ( (rowSize) + ( ((rowSize)+127) >> 7 ) );
  }

  inline ULONG chunkMoreBytes(struct ContextNode* pContextNode) const
  {
    return pContextNode->cn_Size - pContextNode->cn_Scan;
  }

private:
  struct IFFHandle* m_pIffHandle;
};

#endif
