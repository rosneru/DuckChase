#define BOOST_TEST_MAIN
#if !defined( WIN32 )
    #define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>

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
    UBYTE array[] = {0xff, 0xee};
    ShadowMask(array, 16, 10, 1);
}
