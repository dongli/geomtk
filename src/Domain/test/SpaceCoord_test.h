#ifndef __Geomtk_SpaceCoord_test__
#define __Geomtk_SpaceCoord_test__

#include "SpaceCoord.h"

using namespace geomtk;

TEST(SpaceCoord, CopyConstructor) {
    SpaceCoord x(2);
    x(0) = 1.0;
    x(1) = 2.0;
    SpaceCoord y(x);
    ASSERT_EQ(x().size(), y().size());
    ASSERT_EQ(x(0), y(0));
    ASSERT_EQ(x(1), y(1));
}

#endif // __Geomtk_SpaceCoord_test__