#ifndef __GEOMTK_SphereCoord_test__
#define __GEOMTK_SphereCoord_test__

#include "SphereCoord.h"
#include "SphereDomain.h"

using namespace geomtk;

TEST(SphereDomain, CopyConstructor) {
    SphereDomain domain(2);
    SphereCoord x(2);
    x.setCoord(30.0/RAD, 60.0/RAD);
    x.transformToPS(domain);
    SphereCoord y(x);
    ASSERT_EQ(x().size(), y().size());
    ASSERT_EQ(x(0), y(0));
    ASSERT_EQ(x(1), y(1));
    ASSERT_EQ(x.psCoord()[0], y.psCoord()[0]);
    ASSERT_EQ(x.psCoord()[1], y.psCoord()[1]);
    ASSERT_EQ(x.cosLon(), y.cosLon());
    ASSERT_EQ(x.sinLon(), y.sinLon());
    ASSERT_EQ(x.cosLat(), y.cosLat());
    ASSERT_EQ(x.sinLat(), y.sinLat());
}

#endif // __GEOMTK_SphereCoord_test__