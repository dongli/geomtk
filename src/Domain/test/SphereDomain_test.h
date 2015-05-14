#ifndef __GEOMTK_SphereDomain_test__
#define __GEOMTK_SphereDomain_test__

#include "SphereDomain.h"

using namespace geomtk;

class SphereDomainTest : public ::testing::Test {
protected:
    SphereDomain *domain;

    virtual void SetUp() {
        domain = new SphereDomain(2);
        domain->radius() = 1;
    }

    virtual void TearDown() {
        delete domain;
    }
};

TEST_F(SphereDomainTest, SphereCoordOperator) {
    SphereCoord a(2), b(2);
    a.set(1, 2);
    a.transformToPS(*domain);
    b = a;
    ASSERT_EQ(a(0), b(0));
    ASSERT_EQ(a(1), b(1));
    ASSERT_EQ(a.psCoord()[0], b.psCoord()[0]);
    ASSERT_EQ(a.psCoord()[1], b.psCoord()[1]);
    ASSERT_EQ(a.cosLon(), b.cosLon());
    ASSERT_EQ(a.sinLon(), b.sinLon());
    ASSERT_EQ(a.cosLat(), b.cosLat());
    ASSERT_EQ(a.sinLat(), b.sinLat());
}

TEST_F(SphereDomainTest, Constructor) {
    EXPECT_EQ(0.0, domain->axisStart(0));
    EXPECT_EQ(2.0*M_PI, domain->axisEnd(0));
    EXPECT_EQ(PERIODIC, domain->axisStartBndType(0));
    EXPECT_EQ(PERIODIC, domain->axisEndBndType(0));
    EXPECT_EQ(2.0*M_PI, domain->axisSpan(0));
    EXPECT_EQ(-M_PI_2, domain->axisStart(1));
    EXPECT_EQ(M_PI_2, domain->axisEnd(1));
    EXPECT_EQ(POLE, domain->axisStartBndType(1));
    EXPECT_EQ(POLE, domain->axisEndBndType(1));
}

TEST_F(SphereDomainTest, CalculateDistance) {
    SphereCoord x(2), y(2);
    x.set(1.0/3.0*M_PI, 1.0/3.0*M_PI);
    y.set(1.0/3.0*M_PI, -1.0/3.0*M_PI);
    double d = domain->calcDistance(x, y);
    ASSERT_EQ(x(1)-y(1), d);
    d = domain->calcDistance(x.data(), y.data());
    ASSERT_EQ(x(1)-y(1), d);
}

TEST_F(SphereDomainTest, TransformPS) {
    SphereCoord x(2);
    double lon = 0.34*M_PI;
    double lat = -0.4*M_PI_2;
    x.set(lon, lat);
    x.transformToPS(*domain);
    x.transformFromPS(*domain, SOUTH_POLE);
    ASSERT_EQ(lon, x(0));
    ASSERT_EQ(lat, x(1));
}

#endif // __GEOMTK_SphereDomain_test__
