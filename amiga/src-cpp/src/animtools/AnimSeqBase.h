#ifndef ANIM_SEQ_BASE_H
#define ANIM_SEQ_BASE_H

#include <exec/types.h>

/**
 * Encapsulates a generic anim sequence for shapes.
 *
 * @author Uwe Rosner
 * @date 19/01/2020
 */
class AnimSeqBase
{
public:
  int GetWidth();
  int GetWordWidth();
  int GetHeight();
  int GetDepth();

  const char* ErrorMsg();

protected:
  int m_Width;
  int m_WordWidth;
  int m_Height;
  int m_Depth;
  char* m_pErrorMsg;
  const char* m_pInternalError;
  const char* m_pLoadError;
  const char* m_pAllocError;

  AnimSeqBase(int width, int height, int depth);
  virtual ~AnimSeqBase();

  void setErrorMsg(const char* pMsgPart1, const char* pMsgPart2);
  void setErrorMsg(const char* pMsg);
  void freeErrorMsg();
};

#endif
