#ifndef __GEOMTK_SpaceCoord_test__
#define __GEOMTK_SpaceCoord_test__

#include "SpaceCoord.h"

using namespace geomtk;

TEST(SpaceCoord, Basic) {
    SpaceCoord x(2);
    ASSERT_EQ(2, x().size());
    SpaceCoord y(3);
    ASSERT_EQ(3, y().size());
}

TEST(SpaceCoord, CopyConstructor) {
    SpaceCoord x(2);
    x(0) = 1.0;
    x(1) = 2.0;
    SpaceCoord y(x);
    ASSERT_EQ(x().size(), y().size());
    ASSERT_EQ(x(0), y(0));
    ASSERT_EQ(x(1), y(1));
}

#endif // __GEOMTK_SpaceCoord_test__