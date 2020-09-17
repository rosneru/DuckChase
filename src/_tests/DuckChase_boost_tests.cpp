#define BOOST_TEST_MAIN
#if !defined( WIN32 )
    #define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>

#include "Rect.h"
#include "ShadowMask.h"

void* AllocVec( ULONG byteSize, ULONG requirements )
{
  return NULL;
}

void FreeVec( void* memoryBlock )
{
}

BOOST_AUTO_TEST_CASE( test_ShadowMask )
{
  // Bob (4 bytes per row)
  // 1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1
  // 0 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1
  // 0 0 0 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1
  // 0 0 0 0 0 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1
  // 0 0 0 0 0 0 0 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1
  // 0 0 0 0 0 0 0 0  0 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1  1 1 1 1 1 1 1 1
  UBYTE bobMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 
                     0x7F, 0xFF, 0xFF, 0xFF, 
                     0x1F, 0xFF, 0xFF, 0xFF, 
                     0x07, 0xFF, 0xFF, 0xFF, 
                     0x01, 0xFF, 0xFF, 0xFF, 
                     0x00, 0x7F, 0xFF, 0xFF};

  // Sprite (2 bytes per row)
  // 0 0 0 0 0 0 0 0  0 0 0 0 0 1 1 1
  // 0 0 0 0 0 0 0 0  0 0 0 0 0 1 1 1
  // 0 0 0 0 0 0 0 0  0 0 0 1 1 1 0 0
  // 0 0 0 0 0 0 0 0  0 0 1 1 1 0 0 0
  UBYTE spriteMask[] = {0x00, 0x07,
                        0x00, 0x07,
                        0x00, 0x1C,
                        0x00, 0x38};

  const ShadowMask shadowMaskBob(bobMask, 32, 6);
  const ShadowMask shadowMaskSprite(spriteMask, 16, 4);

  // For the rect dimensions see the file 
  // '(WDB) (DEV) (ALGORITHM) (AMIGA) Collsion detection 2.xlsx'
  const Rect bobRectNoCollision(0, 4, 6, 5);
  const Rect spriteRectNoCollision(9, 0, 15, 1);

  const Rect bobRectWithCollision(0, 4, 7, 5);
  const Rect spriteRectWithCollision(8, 0, 15, 1);

  bool does_not_collide = shadowMaskBob.IsCollision(&shadowMaskSprite, 
                                                    bobRectNoCollision,
                                                    spriteRectNoCollision);

  bool does_collide = shadowMaskBob.IsCollision(&shadowMaskSprite, 
                                                bobRectWithCollision,
                                                spriteRectWithCollision);


  BOOST_CHECK_EQUAL(does_not_collide, false);
  BOOST_CHECK_EQUAL(does_collide, true);
}

BOOST_AUTO_TEST_CASE( test_Rect )
{
  Rect a(1, 1, 4, 5);
  BOOST_CHECK_EQUAL(a.Width(), 3);
  BOOST_CHECK_EQUAL(a.Height(), 4);
  BOOST_CHECK_EQUAL(a.Area(), 12);
  BOOST_CHECK_EQUAL(a.HasSize(), true);

  a.SetLeftTop(2, 3);
  BOOST_CHECK_EQUAL(a.Width(), 2);
  BOOST_CHECK_EQUAL(a.Height(), 2);
  BOOST_CHECK_EQUAL(a.Area(), 4);
  BOOST_CHECK_EQUAL(a.HasSize(), true);

  Rect b(1, 1);
  b.SetWidthHeight(3, 4);
  BOOST_CHECK_EQUAL(b.Right(), 4);
  BOOST_CHECK_EQUAL(b.Bottom(), 5);
  BOOST_CHECK_EQUAL(b.Area(), 12);
  BOOST_CHECK_EQUAL(b.HasSize(), true);

  Rect c;
  BOOST_CHECK_EQUAL(c.HasSize(), false);

  Rect d(0, 0, 1, 0);
  BOOST_CHECK_EQUAL(d.HasSize(), false);

  Rect e(0, 0, 2, 1);
  BOOST_CHECK_EQUAL(e.Width(), 2);
  BOOST_CHECK_EQUAL(e.Height(), 1);
  BOOST_CHECK_EQUAL(e.Area(), 2);
  BOOST_CHECK_EQUAL(e.HasSize(), true);
}
