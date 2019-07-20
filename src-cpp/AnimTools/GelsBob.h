#ifndef GELS_GelsBob_H
#define GELS_GelsBob_H

#include <intuition/screens.h>
#include "animtools.h"

#define MAX_IMAGES (8)
/**
 * Encapsulates an blitter object (Bob) of the AmigaOS GELs system.
 *
 * Currently it is using the animtools structures and functions as
 * presented in the RKRM libraries.
 *
 * A bob can contain up to MAX_IMAGES images which all must have the
 * same width, height and depth.
 *
 * The images can be loaded from a memory array, RAW file or by
 * datatype methods (TBD).
 *
 * @author Uwe Rosner
 * @date 11/07/2019
 */
class GelsBob
{
public:
  GelsBob(struct Screen* p_pScreen,
          int p_ImageWidth,
          int p_ImageHeight,
          short p_ImageDepth);

  ~GelsBob();

  /**
   * Loads an image from a RAW file with given path and stores it as
   * next (or first if none already exists) image for this bob.
   *
   * In total up to MAX_IMAGES images can be loaded for each bob.
   *
   * @returns
   * On success true; when image can't be loaded because of wrong path,
   * not enough chip memory or MAX_IMAGES exceeded for this bob, it
   * returns false.
   */
  bool LoadImgFromRawFile(const char* p_pPath);

  /**
   * Loads an image from a given array and stores it as next (or first
   * if none already exists) image for this bob.
   *
   * In total up to MAX_IMAGES images can be loaded for each bob.
   *
   * @returns
   * On success true; when image can't be loaded because of not enough
   * chip memory or MAX_IMAGES exceeded for this bob, it returns false.
   */
  bool LoadImgFromArray(const WORD* p_pAddress);

  /**
   * Getting the struct Bob* of this bob. The bob will be newly created
   * if it doesn't already exists.
   *
   * @returns
   * The struct bob or null if creation has failed.
   */
  struct Bob* Get();

  /**
   * Sets the next image in the bob. If the last loaded image is
   * exceeded, the first image is set again.
   */
  void NextImage();

private:
  struct Screen* m_pScreen;
  struct Bob* m_pBob;

  int m_CurrentImageIndex;
  long m_ImageBufSize;               // Buffer for each image in bytes
  WORD* m_ppImagesArray[MAX_IMAGES]; // Array of pointers to the images

  NEWBOB m_NewBob;

  WORD* createNextImageData();
  void clear();
};

#endif
