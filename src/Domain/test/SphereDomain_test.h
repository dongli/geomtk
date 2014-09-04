#ifndef __Geomtk_SphereDomain_test__
#define __Geomtk_SphereDomain_test__

#include "SphereDomain.h"

using namespace geomtk;

class SphereDomainTest : public ::testing::Test {
protected:
    SphereDomain *domain;

    virtual void SetUp() {
        domain = new SphereDomain(2);
        domain->setRadius(1.0);
    }

    virtual void TearDown() {
        delete domain;
    }
};

TEST_F(SphereDomainTest, SphereCoordOperator) {
    SphereCoord a(2), b(2);

    a(0) = 1.0;
    a(1) = 2.0;

    a.transformToPS(*domain);

    b = a;

    ASSERT_EQ(a(0), b(0));
    ASSERT_EQ(a(1), b(1));
    ASSERT_EQ(a[0], b[0]);
    ASSERT_EQ(a[1], b[1]);
    ASSERT_EQ(a.getCosLon(), b.getCosLon());
    ASSERT_EQ(a.getSinLon(), b.getSinLon());
    ASSERT_EQ(a.getCosLat(), b.getCosLat());
    ASSERT_EQ(a.getSinLat(), b.getSinLat());
}

TEST_F(SphereDomainTest, Constructor) {
    EXPECT_EQ(0.0, domain->getAxisStart(0));
    EXPECT_EQ(2.0*M_PI, domain->getAxisEnd(0));
    EXPECT_EQ(PERIODIC, domain->getAxisStartBndType(0));
    EXPECT_EQ(PERIODIC, domain->getAxisEndBndType(0));
    EXPECT_EQ(2.0*M_PI, domain->getAxisSpan(0));
    EXPECT_EQ(-M_PI_2, domain->getAxisStart(1));
    EXPECT_EQ(M_PI_2, domain->getAxisEnd(1));
    EXPECT_EQ(POLE, domain->getAxisStartBndType(1));
    EXPECT_EQ(POLE, domain->getAxisEndBndType(1));
}

TEST_F(SphereDomainTest, CalculateDistance) {
    SphereCoord x(2), y(2);
    x.setCoord(1.0/3.0*M_PI, 1.0/3.0*M_PI);
    y.setCoord(1.0/3.0*M_PI, -1.0/3.0*M_PI);
    double d = domain->calcDistance(x, y);
    ASSERT_EQ(x(1)-y(1), d);
}

TEST_F(SphereDomainTest, TransformPS) {
    SphereCoord x(2);
    double lon = 0.34*M_PI;
    double lat = -0.4*M_PI_2;
    x.setCoord(lon, lat);
    x.transformToPS(*domain);
    x.transformFromPS(*domain, SOUTH_POLE);
    ASSERT_EQ(lon, x(0));
    ASSERT_EQ(lat, x(1));
}

#endif
