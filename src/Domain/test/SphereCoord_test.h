#ifndef __Geomtk_SphereCoord__
#define __Geomtk_SphereCoord__

#include "SphereDomain.h"

TEST(SphereDomain, CopyConstructor) {
    SphereDomain domain(2);
    SphereCoord x(2);
    x.setCoord(30.0/RAD, 60.0/RAD);
    x.transformToPS(domain);
    SphereCoord y(x);
    ASSERT_EQ(x().size(), y().size());
    ASSERT_EQ(x(0), y(0));
    ASSERT_EQ(x(1), y(1));
    ASSERT_EQ(x[0], y[0]);
    ASSERT_EQ(x[1], y[1]);
    ASSERT_EQ(x.getCosLon(), y.getCosLon());
    ASSERT_EQ(x.getSinLon(), y.getSinLon());
    ASSERT_EQ(x.getCosLat(), y.getCosLat());
    ASSERT_EQ(x.getSinLat(), y.getSinLat());
}

#endif