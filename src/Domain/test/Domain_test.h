#include "Domain.h"
#include "gtest/gtest.h"

class DomainTest : public ::testing::Test {
protected:
    Domain *d2;
    Domain *d3;

    virtual void SetUp() {
        d2 = new Domain;
        d3 = new Domain(3);
    }

    virtual void TearDown() {
        delete d2;
        delete d3;
    }
};

TEST_F(DomainTest, DefaultConstructor) {
    EXPECT_EQ(2, d2->getNumDim());
}

TEST_F(DomainTest, SetAxis) {
    d2->setAxis(0, 0.0, OPEN, 1.0, RIGID);
    d2->setAxis(1, -1.0, RIGID, 1.0, RIGID);
    EXPECT_EQ(0.0, d2->getAxisStart(0));
    EXPECT_EQ(1.0, d2->getAxisEnd(0));
    EXPECT_EQ(OPEN, d2->getAxisStartBndType(0));
    EXPECT_EQ(RIGID, d2->getAxisEndBndType(0));
    EXPECT_EQ(-1.0, d2->getAxisStart(1));
    EXPECT_EQ(1.0, d2->getAxisEnd(1));
    EXPECT_EQ(RIGID, d2->getAxisStartBndType(1));
    EXPECT_EQ(RIGID, d2->getAxisEndBndType(1));
}
