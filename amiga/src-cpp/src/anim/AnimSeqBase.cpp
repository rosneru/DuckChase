#include <clib/exec_protos.h>
#include <exec/memory.h>

#include <string.h>

#include "AnimSeqBase.h"

AnimSeqBase::AnimSeqBase()
  : m_NumFrames(0)
{

}

AnimSeqBase::~AnimSeqBase()
{

}

size_t AnimSeqBase::Width() const
{
  return m_Width;
}

size_t AnimSeqBase::WordWidth() const
{
  return m_WordWidth;
}

size_t AnimSeqBase::Height() const
{
  return m_Height;
}

size_t AnimSeqBase::Depth() const
{
  return m_Depth;
}

size_t AnimSeqBase::NumFrames() const
{
  return m_NumFrames;
}
