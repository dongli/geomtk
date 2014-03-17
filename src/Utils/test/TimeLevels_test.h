#ifndef __Geomtk_TimeLevels_test__
#define __Geomtk_TimeLevels_test__

#include "TimeLevels.h"

using namespace geomtk;

TEST(TimeLevelIndex, Constructors) {
    TimeLevelIndex<3> m;
    
    ASSERT_EQ(0, m.get());
    ASSERT_EQ(0, TimeLevelIndex<3>::currFullIdx);
    ASSERT_EQ(3, TimeLevelIndex<3>::currHalfIdx);
}

TEST(TimeLevelIndex, Operators) {
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

TEST(TimeLevelIndex, Shift) {
    TimeLevelIndex<2> n;
    
    n.shift();
    
    ASSERT_EQ(1, n.get());
    ASSERT_EQ(0, (n+1).get());
    ASSERT_EQ(2, (n+0.5).get());
    
    TimeLevelIndex<2> m = n+1;
    
    ASSERT_EQ(1, (m-1).get());
    ASSERT_EQ(2, (m-0.5).get());
    
    n.reset();
}

TEST(TimeLevelIndex, Reset) {
    TimeLevelIndex<3> n;
    
    n.reset();
    
    ASSERT_EQ(0, n.get());
    ASSERT_EQ(1, (n+1).get());
    ASSERT_EQ(2, (n+2).get());
    ASSERT_EQ(3, (n+0.5).get());
    ASSERT_EQ(4, (n+1.5).get());
}

TEST(TimeLevels, OperatorEqual) {
    TimeLevels<double, 2> a, b;
    a.getLevel(0) = 1;
    a.getLevel(1) = 2;
    b = a;
    ASSERT_EQ(a.getLevel(0), b.getLevel(0));
    ASSERT_EQ(a.getLevel(1), b.getLevel(1));
    
    TimeLevels<vector<int>, 2> c, d;
    c.getLevel(0).push_back(1);
    c.getLevel(0).push_back(2);
    c.getLevel(1).push_back(3);
    d = c;
    ASSERT_EQ(c.getLevel(0).size(), d.getLevel(0).size());
    ASSERT_EQ(c.getLevel(1).size(), d.getLevel(1).size());
    for (int i = 0; i < c.getLevel(0).size(); ++i) {
        ASSERT_EQ(c.getLevel(0)[i], d.getLevel(0)[i]);
    }
    for (int i = 0; i < c.getLevel(1).size(); ++i) {
        ASSERT_EQ(c.getLevel(1)[i], d.getLevel(1)[i]);
    }
}

#endif