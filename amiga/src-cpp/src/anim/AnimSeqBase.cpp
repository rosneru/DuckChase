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

int AnimSeqBase::Width() const
{
  return m_Width;
}

int AnimSeqBase::WordWidth() const
{
  return m_WordWidth;
}

int AnimSeqBase::Height() const
{
  return m_Height;
}

int AnimSeqBase::Depth() const
{
  return m_Depth;
}

int AnimSeqBase::NumFrames() const
{
  return m_NumFrames;
}
