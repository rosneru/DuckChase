#include "OpenIlbmPictureBitMap.h"
#include "OpenImageDataPicture.h"
#include "AnimSeqGelsVSprite.h"

AnimSeqGelsVSprite::AnimSeqGelsVSprite(const char* pFileName, size_t numFrames)
  : AnimSeqGels(numFrames),
    m_pImgLoaderSrc(NULL)
{
  if (pFileName == NULL)
  {
    throw "AnimSeqGelsVSprite: No file name provided.";
  }

  // Load the src file into a bitmap
  OpenIlbmPictureBitMap imgLoaderSrc(pFileName, false, false);

  m_Width = imgLoaderSrc.Width() / numFrames;
  m_WordWidth = ((m_Width + 15) & -16) >> 4;
  m_Height = imgLoaderSrc.Height();
  m_Depth = imgLoaderSrc.Depth();

  // Create a dynamic array for all images according to the number of
  // files
  m_ppFrames = (OpenImageDataPicture**)new OpenImageDataPicture*[numFrames];
  if (m_ppFrames == NULL)
  {
    throw "AnimSeqGelsVSprite: Failed to allocate array memory.";
  }

  // How many words must the pointer be increased to point to the next
  // Image data frame? ==> RKRM, p. 625: Size of a VSprite = Height * 2 *words*
  size_t wordIncrease = m_Height * 2; 

  for(size_t i = 0; i < numFrames; i++)
  {
    // Select the start address of anim image frame in whole anim strip
    // image
    WORD* pSpriteImgData = m_pImgLoaderSrc->GetImageData() + i * wordIncrease;
    
    // Create Image Loader by providing sprite image data
    OpenImageDataPicture* pImg = new OpenImageDataPicture(pSpriteImgData);

    m_ppFrames[i] = pImg;
  }
}


AnimSeqGelsVSprite::~AnimSeqGelsVSprite()
{
  if(m_pImgLoaderSrc != NULL)
  {
    delete m_pImgLoaderSrc;
    m_pImgLoaderSrc = NULL;
  }
}
