#define BOOST_TEST_MAIN
#if !defined( WIN32 )
    #define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>

#include "Rect.h"
// #include "ShadowMask.h"
#include "amiga_types.h"


// void* AllocVec( ULONG byteSize, ULONG requirements )
// {
//   return NULL;
// }

// void FreeVec( void* memoryBlock )
// {
// }

BOOST_AUTO_TEST_CASE( test_InterleavedmaskAlgorithmNew )
{
  UBYTE *pSrcByte, *pDstByte;
  ULONG numMaskCopies, bytesPerRow, i, j, iSrcCol, iSrcRow, iDstRow;

  ULONG m_Width = 16;
  ULONG m_WordWidth = 1;
  ULONG m_Height = 16;
  ULONG m_Depth = 4;


  // Planar mask 16 lines, 16 pixels (2 bytes) per row
  UBYTE pMaskPlanar[] = { 0b00000100, 0b00001111,
                          0b00001100, 0b00111111,
                          0b00011111, 0b01111111,
                          0b00011111, 0b11110101,
                          0b00111111, 0b11000000,
                          0b00111111, 0b10000000,
                          0b01111111, 0b10000000,
                          0b01111111, 0b10100000,
                          0b00111111, 0b11101010,
                          0b00011111, 0b11111110,
                          0b11111111, 0b10001100,
                          0b00011111, 0b00000000,
                          0b11111111, 0b00010000,
                          0b01001111, 0b11110000,
                          0b11110000, 0b00100000,
                          0b11000000, 0b00000000};

  numMaskCopies = m_Depth;
  ULONG planeSizeBytes = sizeof(pMaskPlanar) / sizeof(pMaskPlanar[0]);
  ULONG m_MaskSizeBytes = planeSizeBytes * numMaskCopies;

  BOOST_CHECK_EQUAL(planeSizeBytes, 32);
  BOOST_CHECK_EQUAL(m_MaskSizeBytes, 128);

  UBYTE m_pMask[m_MaskSizeBytes];

 // Convert the planar mask into the interleaved mask
  bytesPerRow = m_WordWidth * 2;
  iSrcRow = 0;
  iDstRow = 0;

  do
  {
    for(iSrcCol = 0; iSrcCol < bytesPerRow; iSrcCol++)
    {
      // Address the correct byte in planar source mask
      // pSrcByte = pMaskBitMap->Planes[0] + ((iSrcRow * bytesPerRow) + iSrcCol);
      pSrcByte = pMaskPlanar + ((iSrcRow * bytesPerRow) + iSrcCol);

      // Address the correct byte in interleaved destination mask
      pDstByte = m_pMask + ((iDstRow * bytesPerRow) + iSrcCol);

      // Perform the needed number of copies vertically in this row
      for(i = 0; i < numMaskCopies; i++)
      {
        *pDstByte = *pSrcByte;
        pDstByte += bytesPerRow;  // Address the byte below the current one
      }
    }

    iSrcRow++;
    iDstRow += numMaskCopies; // Address the row below the last copy destination row
  } 
  while (iSrcRow < m_Height);

  BOOST_CHECK_EQUAL(m_pMask[0], pMaskPlanar[0]);
  BOOST_CHECK_EQUAL(m_pMask[1], pMaskPlanar[1]);
  BOOST_CHECK_EQUAL(m_pMask[2], pMaskPlanar[0]);
  BOOST_CHECK_EQUAL(m_pMask[3], pMaskPlanar[1]);
  BOOST_CHECK_EQUAL(m_pMask[4], pMaskPlanar[0]);
  BOOST_CHECK_EQUAL(m_pMask[5], pMaskPlanar[1]);
  BOOST_CHECK_EQUAL(m_pMask[6], pMaskPlanar[0]);
  BOOST_CHECK_EQUAL(m_pMask[7], pMaskPlanar[1]);

  BOOST_CHECK_EQUAL(m_pMask[8], pMaskPlanar[2]);
  BOOST_CHECK_EQUAL(m_pMask[9], pMaskPlanar[3]);
  BOOST_CHECK_EQUAL(m_pMask[10], pMaskPlanar[2]);
  BOOST_CHECK_EQUAL(m_pMask[11], pMaskPlanar[3]);
  BOOST_CHECK_EQUAL(m_pMask[12], pMaskPlanar[2]);
  BOOST_CHECK_EQUAL(m_pMask[13], pMaskPlanar[3]);
  BOOST_CHECK_EQUAL(m_pMask[14], pMaskPlanar[2]);
  BOOST_CHECK_EQUAL(m_pMask[15], pMaskPlanar[3]);

  BOOST_CHECK_EQUAL(m_pMask[16], pMaskPlanar[4]);
  BOOST_CHECK_EQUAL(m_pMask[17], pMaskPlanar[5]);
  BOOST_CHECK_EQUAL(m_pMask[18], pMaskPlanar[4]);
  BOOST_CHECK_EQUAL(m_pMask[19], pMaskPlanar[5]);
  BOOST_CHECK_EQUAL(m_pMask[20], pMaskPlanar[4]);
  BOOST_CHECK_EQUAL(m_pMask[21], pMaskPlanar[5]);
  BOOST_CHECK_EQUAL(m_pMask[22], pMaskPlanar[4]);
  BOOST_CHECK_EQUAL(m_pMask[23], pMaskPlanar[5]);

  // ...

  BOOST_CHECK_EQUAL(m_pMask[120], pMaskPlanar[30]);
  BOOST_CHECK_EQUAL(m_pMask[121], pMaskPlanar[31]);
  BOOST_CHECK_EQUAL(m_pMask[122], pMaskPlanar[30]);
  BOOST_CHECK_EQUAL(m_pMask[123], pMaskPlanar[31]);
  BOOST_CHECK_EQUAL(m_pMask[124], pMaskPlanar[30]);
  BOOST_CHECK_EQUAL(m_pMask[125], pMaskPlanar[31]);
  BOOST_CHECK_EQUAL(m_pMask[126], pMaskPlanar[30]);
  BOOST_CHECK_EQUAL(m_pMask[127], pMaskPlanar[31]);
}

// BOOST_AUTO_TEST_CASE( test_ShadowMask )
// BOOST_AUTO_TEST_CASE( test_ShadowMask )
// {
//   // Bob (4 bytes per row)
//   // 1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1
//   // 0 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1
//   // 0 0 0 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1
//   // 0 0 0 0 0 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1
//   // 0 0 0 0 0 0 0 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1
//   // 0 0 0 0 0 0 0 0  0 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1
//   UBYTE bobMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 
//                      0x7F, 0xFF, 0xFF, 0xFF, 
//                      0x1F, 0xFF, 0xFF, 0xFF, 
//                      0x07, 0xFF, 0xFF, 0xFF, 
//                      0x01, 0xFF, 0xFF, 0xFF, 
//                      0x00, 0x7F, 0xFF, 0xFF};

//   // Sprite (2 bytes per row)
//   // 0 0 0 0 0 0 0 0  0 0 0 0 0 1 1 1
//   // 0 0 0 0 0 0 0 0  0 0 0 0 0 1 1 1
//   // 0 0 0 0 0 0 0 0  0 0 0 1 1 1 0 0
//   // 0 0 0 0 0 0 0 0  0 0 1 1 1 0 0 0
//   UBYTE spriteMask[] = {0x00, 0x07,
//                         0x00, 0x07,
//                         0x00, 0x1C,
//                         0x00, 0x38};

//   const ShadowMask shadowMaskBob(bobMask, 32, 6);
//   const ShadowMask shadowMaskSprite(spriteMask, 16, 4);

//   // For the rect dimensions see the file 
//   // '(WDB) (DEV) (ALGORITHM) (AMIGA) Collsion detection 2.xlsx'
//   const Rect bobRectNoCollision(0, 4, 6, 5);
//   const Rect spriteRectNoCollision(9, 0, 15, 1);

//   const Rect bobRectWithCollision(0, 4, 7, 5);
//   const Rect spriteRectWithCollision(8, 0, 15, 1);

//   bool does_not_collide = shadowMaskBob.IsCollision(&shadowMaskSprite, 
//                                                     bobRectNoCollision,
//                                                     spriteRectNoCollision);

//   bool does_collide = shadowMaskBob.IsCollision(&shadowMaskSprite, 
//                                                 bobRectWithCollision,
//                                                 spriteRectWithCollision);


//   BOOST_CHECK_EQUAL(does_not_collide, false);
//   BOOST_CHECK_EQUAL(does_collide, true);
// }

// BOOST_AUTO_TEST_CASE( test_Rect )
// {
//   Rect a(1, 1, 4, 5);
//   BOOST_CHECK_EQUAL(a.Width(), 4);
//   BOOST_CHECK_EQUAL(a.Height(), 5);
//   BOOST_CHECK_EQUAL(a.Area(), 20);
//   BOOST_CHECK_EQUAL(a.HasSize(), true);

//   a.SetLeftTop(2, 3);
//   BOOST_CHECK_EQUAL(a.Width(), 3);
//   BOOST_CHECK_EQUAL(a.Height(), 3);
//   BOOST_CHECK_EQUAL(a.Area(), 9);
//   BOOST_CHECK_EQUAL(a.HasSize(), true);

//   Rect b(1, 1);
//   b.SetWidthHeight(3, 4);
//   BOOST_CHECK_EQUAL(b.Right(), 3);
//   BOOST_CHECK_EQUAL(b.Bottom(), 4);
//   BOOST_CHECK_EQUAL(b.Area(), 12);
//   BOOST_CHECK_EQUAL(b.HasSize(), true);
  
//   BOOST_CHECK_EQUAL(b.Bottom() - b.Top() + 1, b.Height());

//   Rect c;
//   BOOST_CHECK_EQUAL(c.HasSize(), false);

//   Rect d(0, 0, 1, 0);
//   BOOST_CHECK_EQUAL(d.HasSize(), false);

//   Rect e(0, 0, 2, 1);
//   BOOST_CHECK_EQUAL(e.Width(), 3);
//   BOOST_CHECK_EQUAL(e.Height(), 2);
//   BOOST_CHECK_EQUAL(e.Area(), 6);
//   BOOST_CHECK_EQUAL(e.HasSize(), true);
// }
