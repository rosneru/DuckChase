#include <proto/exec.h>
#include <proto/dos.h>

#include <graphics/gels.h>

#include "animtools_proto.h"
#include "GelsBob.h"


GelsBob::GelsBob(struct Screen* p_pScreen, short p_ScreenDepth)
  : m_pScreen(p_pScreen),
    m_pBob(NULL),
    m_pImageData(NULL)
{
  m_NewBob.nb_Image = NULL;                // Image data
  m_NewBob.nb_WordWidth = 0;               // Bob width (in number of 16-pixel-words)
  m_NewBob.nb_LineHeight = 0;              // Bob height in lines
  m_NewBob.nb_ImageDepth = 0;              // Image depth
  m_NewBob.nb_PlanePick = 0;               // Planes that get image data
  m_NewBob.nb_PlaneOnOff = 0;              // Unused planes to turn on
  m_NewBob.nb_BFlags = SAVEBACK | OVERLAY; // Bog flags
  m_NewBob.nb_DBuf = 0;                    // DoubleBuffering. Set to '1' to activate.
  m_NewBob.nb_RasDepth = p_ScreenDepth;    // Depth of the raster
  m_NewBob.nb_X = 0;                       // Initial x position
  m_NewBob.nb_Y = 0;                       // Initial y position
  m_NewBob.nb_HitMask = 0;                 // Hit mask
  m_NewBob.nb_MeMask = 0;                  // Me mask
}


GelsBob::~GelsBob()
{
  clear();
}


bool GelsBob::CreateFromRawFile(const char* p_pPath,
                                int p_Width,
                                int p_Height,
                                short p_Depth)
{
  //
  // Calculate dimensions and values
  //
  int numWords = ((p_Width + 15) & -16) >> 4;
  long bufSizeInBytes = numWords * p_Height * p_Depth * 2;

  //
  //  Open the file and calculate its sie using Seek()
  //
  BPTR fileHandle = Open(p_pPath, MODE_OLDFILE);
  if(fileHandle == NULL)
  {
    return false;
  }

  Seek(fileHandle, 0, OFFSET_END);
  long fileSize = Seek(fileHandle, 0, OFFSET_BEGINNING);

  if(fileSize != bufSizeInBytes)
  {
    return false;
  }

  //
  // Allocate chip memory
  //
  m_pImageData = (WORD*) AllocVec(bufSizeInBytes, MEMF_CHIP|MEMF_CLEAR);

  //
  // Read the file data into target chip memory buffer
  //
  if(Read(fileHandle, m_pImageData, bufSizeInBytes) != 1)
  {
    Close(fileHandle);
    return false;
  }

  Close(fileHandle);

  //
  // Fill the struct NewBob
  //
  m_NewBob.nb_Image = m_pImageData;
  m_NewBob.nb_WordWidth = numWords;
  m_NewBob.nb_LineHeight = p_Height;
  m_NewBob.nb_ImageDepth = p_Depth;

  //
  // Finally create the Bob
  //
  m_pBob = makeBob(&m_NewBob);

  return m_pBob != NULL;
}


bool GelsBob::CreateFromArray(const WORD* p_pAddress,
                              int p_Width,
                              int p_Height,
                              short p_Depth)
{
  //
  // Calculate dimensions and values
  //
  int numWords = ((p_Width + 15) & -16) >> 4;
  long bufSizeInBytes = numWords * p_Height * p_Depth * 2;

  //
  // Allocate chip memory
  //
  m_pImageData = (WORD*) AllocVec(bufSizeInBytes, MEMF_CHIP|MEMF_CLEAR);

  //
  // Copy source data to target chip memory buffer
  //
  for(int i = 0; i < (bufSizeInBytes / 2); i++)
  {
    m_pImageData[i] = p_pAddress[i];
  }

  //
  // Fill the struct NewBob
  //
  m_NewBob.nb_Image = m_pImageData;
  m_NewBob.nb_WordWidth = numWords;
  m_NewBob.nb_LineHeight = p_Height;
  m_NewBob.nb_ImageDepth = p_Depth;

  //
  // Finally create the Bob
  //
  m_pBob = makeBob(&m_NewBob);

  return m_pBob != NULL;
}


struct Bob* GelsBob::GetBob()
{
  return m_pBob;
}

void GelsBob::clear()
{
  if(m_pBob != NULL)
  {
    freeBob(m_pBob, m_NewBob.nb_RasDepth);
    m_pBob = NULL;
  }

  if(m_pImageData != NULL)
  {
    FreeVec(m_pImageData);
    m_pImageData = NULL;
  }
}
