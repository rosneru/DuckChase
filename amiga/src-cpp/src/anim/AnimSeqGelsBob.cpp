#include "clib/exec_protos.h"
#include "clib/graphics_protos.h"
#include "exec/memory.h"

#include "IlbmBitmap.h"
#include "ImageDataPicture.h"
#include "AnimSeqGelsBob.h"

AnimSeqGelsBob::AnimSeqGelsBob(const char* pFileName, size_t numFrames)
  : AnimSeqGels(numFrames)
{
  if (pFileName == NULL)
  {
    throw "AnimSeqGelsBob: No file name provided.";
  }

  // Load the src file into a bitmap
  IlbmBitmap srcIlbmBitmap(pFileName, false, false);

  m_Width = srcIlbmBitmap.Width() / numFrames;
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
  m_Height = srcIlbmBitmap.Height();
  m_Depth = srcIlbmBitmap.Depth();

  // Create a dynamic array for all frames
  m_ppFrames = (ImageDataPicture**)new ImageDataPicture*[numFrames];
  if (m_ppFrames == NULL)
  {
    throw "AnimSeqGelsBob: Failed to allocate array memory.";
  }

  // Create a destination BitMap to blit each destination frame image into
  size_t frameWidth = srcIlbmBitmap.Width() / numFrames;
  m_pFrameBitMap = AllocBitMap(frameWidth,
                               srcIlbmBitmap.Height(),
                               srcIlbmBitmap.Depth(),
                               BMF_CLEAR,
                               NULL);

  // Create all frames by copying an area of src bitmap
  for(size_t i = 0; i < numFrames; i++)
  {

    size_t xStart = i * frameWidth;
    BltBitMap(srcIlbmBitmap.GetBitMap(),
              xStart,
              0,
              m_pFrameBitMap,
              0,
              0,
              frameWidth - 1,
              srcIlbmBitmap.Height() - 1,
              0Xc0,
              0xff,
              NULL);
    
    ImageDataPicture* pImg = new ImageDataPicture(m_pFrameBitMap, 0, 1);
    
    m_ppFrames[i] = pImg;
    m_ppShadowMasks[i] = new ShadowMask(m_pFrameBitMap);
  }

  FreeBitMap(m_pFrameBitMap);
  m_pFrameBitMap = NULL;
}


AnimSeqGelsBob::~AnimSeqGelsBob()
{

}
