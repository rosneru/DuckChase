#include "clib/exec_protos.h"
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
  IlbmBitmap imgLoaderSrc(pFileName, false, false);

  m_Width = imgLoaderSrc.Width() / numFrames;
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
  m_Height = imgLoaderSrc.Height();
  m_Depth = imgLoaderSrc.Depth();

  // Create a dynamic array for all frames
  m_ppFrames = (ImageDataPicture**)new ImageDataPicture*[numFrames];
  if (m_ppFrames == NULL)
  {
    throw "AnimSeqGelsBob: Failed to allocate array memory.";
  }

  // Create the shadow mask array
  m_ppShadowMasks = (UBYTE**) AllocVec(numFrames * sizeof(UBYTE**), 
                                       MEMF_ANY); // TODO MEMF_CHIP for Blitter use
  if (m_ppShadowMasks == NULL)
  {
    throw "AnimSeqGelsBob: Failed to allocate shadow mask array memory.";
  }

  // Load all files by copying a area of src bitmap
  for(size_t i = 0; i < numFrames; i++)
  {
    ImageDataPicture* pImg = new ImageDataPicture(imgLoaderSrc.GetBitMap(), 
                                                  i * m_Width, numFrames);

    m_ppFrames[i] = pImg;
    // m_ppShadowMasks[i] = createShadowMask
  }
}


AnimSeqGelsBob::~AnimSeqGelsBob()
{

}
