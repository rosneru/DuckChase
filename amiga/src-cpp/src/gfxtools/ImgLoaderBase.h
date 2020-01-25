#ifndef IMG_LOADER_BASE_H
#define IMG_LOADER_BASE_H

#include <exec/types.h>

/**
 * Encapsulates a generic image loader.
 *
 * @author Uwe Rosner
 * @date 21/01/2020
 */
class ImgLoaderBase
{
public:
  int GetWidth();
  int GetHeight();
  int GetDepth();

  const char* ErrorMsg();

protected:
  int m_Width;
  int m_WordWidth;
  int m_Height;
  int m_Depth;
  char* m_pErrorMsg;
  const char* m_pAllocError;
  const char* m_pInternalError;
  const char* m_pLoadError;

  ImgLoaderBase(int width, int height, int depth);
  virtual ~ImgLoaderBase();

  void setErrorMsg(const char* pMsgPart1, const char* pMsgPart2);
  void setErrorMsg(const char* pMsg);
  void freeErrorMsg();
};

#endif
