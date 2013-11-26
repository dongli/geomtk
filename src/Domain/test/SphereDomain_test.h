#include "SphereDomain.h"

class SphereDomainTest : public ::testing::Test {
protected:
    SphereDomain *sphere;

    virtual void SetUp() {
        sphere = new SphereDomain(2);
    }

    virtual void TearDown() {
        delete sphere;
    }
};

TEST_F(SphereDomainTest, Constructor) {
    EXPECT_EQ(0.0, sphere->getAxisStart(0));
    EXPECT_EQ(2.0*M_PI, sphere->getAxisEnd(0));
    EXPECT_EQ(PERIODIC, sphere->getAxisStartBndType(0));
    EXPECT_EQ(PERIODIC, sphere->getAxisEndBndType(0));
    EXPECT_EQ(2.0*M_PI, sphere->getAxisSpan(0));
    EXPECT_EQ(-M_PI_2, sphere->getAxisStart(1));
    EXPECT_EQ(M_PI_2, sphere->getAxisEnd(1));
    EXPECT_EQ(POLE, sphere->getAxisStartBndType(1));
    EXPECT_EQ(POLE, sphere->getAxisEndBndType(1));
}

TEST_F(SphereDomainTest, ReverseLatitudeAxis) {
    sphere->reverseLatitudeAxis();
    EXPECT_EQ(M_PI_2, sphere->getAxisStart(1));
    EXPECT_EQ(-M_PI_2, sphere->getAxisEnd(1));
}
