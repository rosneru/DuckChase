#include "IlbmBitMap.h"
#include "ImageDataPicture.h"
#include "AnimSeqGelsBob.h"

AnimSeqGelsBob::AnimSeqGelsBob(const char* pFileName, size_t numFrames)
{
  if (pFileName == NULL)
  {
    throw "AnimSeqGelsBob: No file name provided.";
  }

  // Load the src file into a bitmap
  IlbmBitMap imgLoaderSrc(pFileName, false, false);

  m_Width = imgLoaderSrc.Width() / numFrames;
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
  m_Height = imgLoaderSrc.Height();
  m_Depth = imgLoaderSrc.Depth();

  // Create a dynamic array for all images according to the number of
  // files
  m_ppFrames = (ImageDataPicture**)new ImageDataPicture*[numFrames];
  if (m_ppFrames == NULL)
  {
    throw "AnimSeqGelsBob: Failed to allocate array memory.";
  }

  // Load all files by copying a area of src bitmap
  for(size_t i = 0; i < numFrames; i++)
  {
    ImageDataPicture* pImg = new ImageDataPicture(imgLoaderSrc.GetBitMap(), 
                                                  i * m_Width, numFrames);

    m_ppFrames[i] = pImg;
  }

  m_NumFrames = numFrames;
}


AnimSeqGelsBob::~AnimSeqGelsBob()
{

}
