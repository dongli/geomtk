#ifndef __Geomtk_SpaceCoord__
#define __Geomtk_SpaceCoord__

#include "Domain.h"

TEST(SpaceCoord, CopyConstructor) {
    SpaceCoord x(2);
    x(0) = 1.0;
    x(1) = 2.0;
    SpaceCoord y(x);
    ASSERT_EQ(x(0), y(0));
    ASSERT_EQ(x(1), y(1));
}

#endif