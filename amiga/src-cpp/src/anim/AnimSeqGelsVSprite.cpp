
#include "ImageDataPicture.h"
#include "AnimSeqGelsVSprite.h"

AnimSeqGelsVSprite::AnimSeqGelsVSprite(const char* pFileName, size_t numFrames)
  : m_pImgLoaderSrc(NULL)
{
  if (pFileName == NULL)
  {
    throw "AnimSeqGelsVSprite: No file name provided.";
  }


  m_pImgLoaderSrc = new ImageDataPicture(pFileName,
                                         m_Width * numFrames, 
                                         m_Height, 
                                         m_Depth);

  // Create a dynamic array for all images according to the number of
  // files
  m_ppFrames = (ImageDataPicture**)new ImageDataPicture*[numFrames];
  if (m_ppFrames == NULL)
  {
    throw "AnimSeqGelsVSprite: Failed to allocate array memory.";
  }

  // How many words must the pointer be increased to point to the next
  // Image data file?
  size_t wordIncrease = m_Height * 2; // RKRM, p. 625: Size of a VSprite = Height * 2 *words*

  for(size_t i = 0; i < numFrames; i++)
  {
    // Select the start address of anim image frame in whole anim strip
    // image
    WORD* pSpriteImgData = m_pImgLoaderSrc->GetImageData() + i * wordIncrease;
    
    // Create Image Loader by providing sprite image data
    ImageDataPicture* pImg = new ImageDataPicture(pSpriteImgData);

    m_ppFrames[i] = pImg;
  }

  m_NumFrames = numFrames;
}


AnimSeqGelsVSprite::~AnimSeqGelsVSprite()
{
  if(m_pImgLoaderSrc != NULL)
  {
    delete m_pImgLoaderSrc;
    m_pImgLoaderSrc = NULL;
  }
}
