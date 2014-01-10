#ifndef __Geomtk_TimeLevels_test__
#define __Geomtk_TimeLevels_test__

#include "TimeLevels.h"

using namespace geomtk;

TEST(TimeLevels, Constructors) {
    TimeLevelIndex<3> m;
    
    ASSERT_EQ(0, m.get());
    ASSERT_EQ(0, TimeLevelIndex<3>::currFullIdx);
    ASSERT_EQ(3, TimeLevelIndex<3>::currHalfIdx);
}

TEST(TimeLevels, Operators) {
    TimeLevelIndex<3> n;

    ASSERT_EQ(0, n.get());
    ASSERT_EQ(1, (n+1).get());
    ASSERT_EQ(2, (n+2).get());
    ASSERT_EQ(3, (n+0.5).get());
    ASSERT_EQ(4, (n+1.5).get());
    
    ASSERT_EQ(true, n.isCurrentIndex());
    ASSERT_EQ(false, (n+1).isCurrentIndex());
    
    ASSERT_EQ(3, (n+1-0.5).get());
    ASSERT_EQ(4, (n+2-0.5).get());
    ASSERT_EQ(3, (n+2-1.5).get());
    ASSERT_EQ(1, (n+2-1).get());
}

TEST(TimeLevels, Shift) {
    TimeLevelIndex<2> n;
    
    n.shift();
    
    ASSERT_EQ(1, n.get());
    ASSERT_EQ(0, (n+1).get());
    ASSERT_EQ(2, (n+0.5).get());
    
    TimeLevelIndex<2> m = n+1;
    
    ASSERT_EQ(1, (m-1).get());
    
    n.reset();
}

TEST(TimeLevels, Reset) {
    TimeLevelIndex<3> n;
    
    n.reset();
    
    ASSERT_EQ(0, n.get());
    ASSERT_EQ(1, (n+1).get());
    ASSERT_EQ(2, (n+2).get());
    ASSERT_EQ(3, (n+0.5).get());
    ASSERT_EQ(4, (n+1.5).get());
}

#endif