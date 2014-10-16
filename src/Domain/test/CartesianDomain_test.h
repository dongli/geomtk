#ifndef __Geomtk_CartesianDomain_test__
#define __Geomtk_CartesianDomain_test__

#include "CartesianDomain.h"

using namespace geomtk;

class CartesianDomainTest : public ::testing::Test {
protected:
    CartesianDomain *d2;
    CartesianDomain *d3;

    virtual void SetUp() {
        d2 = new CartesianDomain;
        d3 = new CartesianDomain(HEIGHT);
    }

    virtual void TearDown() {
        delete d2;
        delete d3;
    }
};

TEST_F(CartesianDomainTest, Basics) {
    EXPECT_EQ(2, d2->numDim());
    EXPECT_EQ(3, d3->numDim());
}

TEST_F(CartesianDomainTest, SetAxis) {
    d2->setAxis(0, "x", "x", "1",  0.0,  OPEN, 1.0, RIGID);
    d2->setAxis(1, "y", "y", "1", -1.0, RIGID, 1.0, RIGID);
    EXPECT_EQ(0.0, d2->axisStart(0));
    EXPECT_EQ(1.0, d2->axisEnd(0));
    EXPECT_EQ(OPEN, d2->axisStartBndType(0));
    EXPECT_EQ(RIGID, d2->axisEndBndType(0));
    EXPECT_EQ(-1.0, d2->axisStart(1));
    EXPECT_EQ(1.0, d2->axisEnd(1));
    EXPECT_EQ(RIGID, d2->axisStartBndType(1));
    EXPECT_EQ(RIGID, d2->axisEndBndType(1));
}

TEST_F(CartesianDomainTest, DifferenceCoordinate) {
    d2->setAxis(0, "x", "x", "m", -1.0, PERIODIC, 1.0, PERIODIC);
    d2->setAxis(1, "y", "y", "m", -1.0, PERIODIC, 1.0, PERIODIC);
    SpaceCoord x(2), y(2);
    x(0) = -0.9; x(1) = -0.9;
    y(0) =  0.9; y(1) =  0.9;
    vec dxy = d2->diffCoord(x, y);
    ASSERT_GT(1.0e-15, fabs(0.2-dxy(0)));
    ASSERT_GT(1.0e-15, fabs(0.2-dxy(1)));
}

TEST_F(CartesianDomainTest, CalculateDistance) {
    d2->setAxis(0, "x", "x", "m", -1.0, PERIODIC, 1.0, PERIODIC);
    d2->setAxis(1, "y", "y", "m", -1.0, PERIODIC, 1.0, PERIODIC);
    SpaceCoord x(2), y(2);
    x(0) = -0.9; x(1) = -0.9;
    y(0) =  0.9; y(1) =  0.9;
    ASSERT_GT(1.0e-15, fabs(0.2*sqrt(2)-d2->calcDistance(x, y)));
}

#endif // __Geomtk_CartesianDomain_test__
