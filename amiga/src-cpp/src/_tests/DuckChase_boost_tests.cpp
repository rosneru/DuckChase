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
