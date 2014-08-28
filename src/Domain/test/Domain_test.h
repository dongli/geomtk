#ifndef __Geomtk_Domain_test__
#define __Geomtk_Domain_test__

#include "Domain.h"

using namespace geomtk;

class DomainTest : public ::testing::Test {
protected:
    Domain *d2;
    Domain *d3;

    virtual void SetUp() {
        d2 = new Domain;
        d3 = new Domain(HEIGHT);
    }

    virtual void TearDown() {
        delete d2;
        delete d3;
    }
};

TEST_F(DomainTest, Constructor) {
    EXPECT_EQ(2, d2->getNumDim());
    EXPECT_EQ(3, d3->getNumDim());
}

TEST_F(DomainTest, SetAxis) {
    d2->setAxis(0, "x", "x", "1",  0.0,  OPEN, 1.0, RIGID);
    d2->setAxis(1, "y", "y", "1", -1.0, RIGID, 1.0, RIGID);
    EXPECT_EQ(0.0, d2->getAxisStart(0));
    EXPECT_EQ(1.0, d2->getAxisEnd(0));
    EXPECT_EQ(OPEN, d2->getAxisStartBndType(0));
    EXPECT_EQ(RIGID, d2->getAxisEndBndType(0));
    EXPECT_EQ(-1.0, d2->getAxisStart(1));
    EXPECT_EQ(1.0, d2->getAxisEnd(1));
    EXPECT_EQ(RIGID, d2->getAxisStartBndType(1));
    EXPECT_EQ(RIGID, d2->getAxisEndBndType(1));
}

#endif // __Geomtk_Domain_test__
