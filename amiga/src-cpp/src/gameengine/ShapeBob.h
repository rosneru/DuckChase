#ifndef SHAPE_BOB_H
#define SHAPE_BOB_H

#include <intuition/screens.h>
#include "animtools.h"
#include "ShapeBase.h"

#define MAX_IMAGES (8)
/**
 * Encapsulates an animatable graphics object using the blitter hardware
 * (Gels system) related functions of AmigaOS graphics.libary.
 *
 * Currently it is using the animtools structures and functions as
 * presented in the RKRM libraries.
 *
 * A bob can contain up to MAX_IMAGES images which all must have the
 * same width, height and depth.
 *
 * The images can be loaded from a memory array, RAW file or by datatype
 * methods (TBD).
 *
 * @author Uwe Rosner
 * @date 11/07/2019
 */
class ShapeBob : public ShapeBase
{
public:
  ShapeBob(short p_pViewDepth,
           int p_ImageWidth,
           int p_ImageHeight,
           short p_ImageDepth);

  virtual ~ShapeBob();

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
  bool AddRawImage(const char* p_pPath);

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
   * Bobs are onyl visible in one deicated RastPort..
   */
  void AddToRastPort(struct RastPort* pRastPort);


  //
  // Implementing part of the interface IEntity. Init(), Update(),
  // XSpeed_pps() and YSpeed_pps() must be implemented in derived
  // classes.
  //

  int XPos() const;
  int YPos() const;
  int Width() const;
  int Height() const;
  void Move(int x, int y);

  void SetInvisible();
  void SetVisible();
  bool IsVisible() const;

  void NextImage();

private:
  int m_ImageWidth;
  int m_ImageHeight;

  struct Bob* m_pBob;
  struct RastPort* m_pRastPort;

  bool m_bIsVisible;

  WORD* m_pImageShadow;

  int m_CurrentImageIndex;
  long m_ImageBufSize;               // Buffer for each image in bytes
  WORD* m_ppImagesArray[MAX_IMAGES]; // Array of pointers to the images

  NEWBOB m_NewBob;

  WORD* createNextImageData();
  void clear();
};

#endif
