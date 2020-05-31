#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <dos/dos.h>
#include <exec/memory.h>
#include <graphics/gfx.h>

#include <stddef.h>

#include "ImageDataPicture.h"

ImageDataPicture::ImageDataPicture(const char* pFileName, 
                                   ULONG width, 
                                   ULONG height, 
                                   ULONG depth)
  : PictureBase(),
    m_FileHandle(0),
    m_pImageData(NULL),
    m_pExternalImageData(NULL),
    m_Width(width),
    m_WordWidth(((width + 15) & -16) >> 4),
    m_Height(height),
    m_Depth(depth)
{
  if (pFileName == NULL)
  {
    throw "ImageDataPicture: No file name provided.";
  }

  m_FileHandle = Open(pFileName, MODE_OLDFILE);
  if (m_FileHandle == 0)
  {
    throw "ImageDataPicture: Failed to open file.";
  }

  // Determine needed memory size for image data
  LONG bufSizeBytes = m_WordWidth * 2 * height * depth;

  if(m_pImageData == NULL)
  {
    m_pImageData = (WORD*) AllocVec(bufSizeBytes, MEMF_CHIP|MEMF_CLEAR);
    if(m_pImageData == NULL)
    {
      throw "ImageDataPicture: Failed allocate chip mem for file.";
    }
  }

  // Read the file data into target chip memory buffer
  if (Read(m_FileHandle, m_pImageData, bufSizeBytes) != bufSizeBytes)
  {
    throw "ImageDataPicture: failed to read file.";
  }

  Close(m_FileHandle);
  m_FileHandle = 0;
}


ImageDataPicture::ImageDataPicture(struct BitMap* pSrcBitmap, 
                                   ULONG xStart, 
                                   ULONG numFrames)
  : PictureBase(),
    m_FileHandle(0),
    m_pImageData(NULL),
    m_pExternalImageData(NULL)
{
  if(pSrcBitmap == NULL)
  {
    throw "ImageDataPicture: No source BitMap provided.";
  }

  ULONG width = GetBitMapAttr(pSrcBitmap, BMA_WIDTH) / numFrames;
  ULONG wordWidth = ((width + 15) & -16) >> 4;
  ULONG height = GetBitMapAttr(pSrcBitmap, BMA_HEIGHT);
  ULONG depth = GetBitMapAttr(pSrcBitmap, BMA_DEPTH);

  // Create a destination BitMap
  struct BitMap bitmap;
  InitBitMap(&bitmap, depth, width, height);

  // Allocate memory for the planes of destination Bitmap
  size_t planeSize = RASSIZE(width, height);
  size_t bufSizeBytes = planeSize * depth;

  m_pImageData = (WORD*)AllocVec(bufSizeBytes, MEMF_CHIP|MEMF_CLEAR);
  if(m_pImageData == NULL)
  {
    throw "ImageDataPicture: Failed allocate chip mem for image data";
  }

  // Manually set all plane pointers to the dedicated area of 
  // destination Bitmap
  PLANEPTR ptr = (PLANEPTR)m_pImageData;
  for(size_t i = 0;i < depth;i++)
  {
    bitmap.Planes[i] = ptr;
    ptr += planeSize;
  }

  // Blit given portion of the source Bitmap to destination BitMap
  // After this the image data m_pImageData also is filled with
  // the corresponding data.
  BltBitMap(pSrcBitmap, 
            xStart,
            0,
            &bitmap,
            0,
            0,
            width,
            height,
            0xC0, 
            0xFF, 
            NULL);

  m_Width = width;
  m_WordWidth = wordWidth;
  m_Height = height;
  m_Depth = depth;
}


ImageDataPicture::ImageDataPicture(WORD* pExternalImageData)
  : PictureBase(),
    m_FileHandle(0),
    m_pImageData(NULL),
    m_pExternalImageData(pExternalImageData),
    m_Width(0),
    m_WordWidth(0),
    m_Height(0),
    m_Depth(0)
{

}

ImageDataPicture::~ImageDataPicture()
{
  if(m_pImageData != NULL)
  {
    FreeVec(m_pImageData);
    m_pImageData = NULL;
  }

  if(m_FileHandle != 0)
  {
    Close(m_FileHandle);
  }
}






WORD* ImageDataPicture::GetImageData()
{
  if(m_pImageData != NULL)
  {
    return m_pImageData;
  }
  else
  {
    return m_pExternalImageData;
  }
}


long ImageDataPicture::Width()
{
  return m_Width;
}

long ImageDataPicture::WordWidth()
{
  return m_Height;
}

long ImageDataPicture::Height()
{
  return m_Depth;
}

long ImageDataPicture::Depth()
{
  return m_WordWidth;
}
