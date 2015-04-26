#ifndef __GEOMTK_Time_test__
#define __GEOMTK_Time_test__

#include "TimeManager.h"

using namespace geomtk;

TEST(Time, Constructor) {
    Time a;

    ASSERT_FALSE(a.useLeap);
    ASSERT_EQ(1, a.year);
    ASSERT_EQ(1, a.month);
    ASSERT_EQ(1, a.day);
    ASSERT_EQ(0, a.hour);
    ASSERT_EQ(0, a.minute);
    ASSERT_EQ(0, a.second);
}

TEST(Time, Operations) {
    Time a;
    ASSERT_EQ(31, a.daysOfMonth(1));
    ASSERT_EQ(28, a.daysOfMonth(2));
    ASSERT_EQ(31, a.daysOfMonth(3));
    ASSERT_EQ(30, a.daysOfMonth(4));
    ASSERT_EQ(31, a.daysOfMonth(5));
    ASSERT_EQ(30, a.daysOfMonth(6));
    ASSERT_EQ(31, a.daysOfMonth(7));
    ASSERT_EQ(31, a.daysOfMonth(8));
    ASSERT_EQ(30, a.daysOfMonth(9));
    ASSERT_EQ(31, a.daysOfMonth(10));
    ASSERT_EQ(30, a.daysOfMonth(11));
    ASSERT_EQ(31, a.daysOfMonth(12));
    
    ASSERT_EQ(0,   a.daysBeforeMonth(1));
    ASSERT_EQ(31,  a.daysBeforeMonth(2));
    ASSERT_EQ(59,  a.daysBeforeMonth(3));
    ASSERT_EQ(90,  a.daysBeforeMonth(4));
    ASSERT_EQ(120, a.daysBeforeMonth(5));
    ASSERT_EQ(151, a.daysBeforeMonth(6));
    ASSERT_EQ(181, a.daysBeforeMonth(7));
    ASSERT_EQ(212, a.daysBeforeMonth(8));
    ASSERT_EQ(243, a.daysBeforeMonth(9));
    ASSERT_EQ(273, a.daysBeforeMonth(10));
    ASSERT_EQ(304, a.daysBeforeMonth(11));
    ASSERT_EQ(334, a.daysBeforeMonth(12));

    ASSERT_EQ(334, a.daysAfterMonth(1));
    ASSERT_EQ(306, a.daysAfterMonth(2));
    ASSERT_EQ(275, a.daysAfterMonth(3));
    ASSERT_EQ(245, a.daysAfterMonth(4));
    ASSERT_EQ(214, a.daysAfterMonth(5));
    ASSERT_EQ(184, a.daysAfterMonth(6));
    ASSERT_EQ(153, a.daysAfterMonth(7));
    ASSERT_EQ(122, a.daysAfterMonth(8));
    ASSERT_EQ(92,  a.daysAfterMonth(9));
    ASSERT_EQ(61,  a.daysAfterMonth(10));
    ASSERT_EQ(31,  a.daysAfterMonth(11));
    ASSERT_EQ(0,   a.daysAfterMonth(12));

    a.year = 1990;
    a.month = 1;
    a.day = 10;
    a.hour = 0;
    a.minute = 0;
    a.second = 13.4;

    Time b = a-22*86400;
    
    ASSERT_EQ(a.year-1, b.year);
    ASSERT_EQ(a.prevMonth(), b.month);
    ASSERT_EQ(19, b.day);
    ASSERT_EQ(0, b.hour);
    ASSERT_EQ(0, b.minute);
    ASSERT_EQ(a.second, b.second);

    ASSERT_EQ(22*86400,  a.seconds(b));
    ASSERT_EQ(-22*86400, b.seconds(a));
}

TEST(Time, Operators) {
    Time a, b;

    a.year = 1990;
    a.month = 1;
    a.day = 10;
    a.hour = 0;
    a.minute = 0;
    a.second = 13.4;

    b.year = 1991;
    b.month = 10;
    b.day = 1;
    b.minute = 13;
    b.second = 10;

    ASSERT_LT(a, b);
    ASSERT_LE(a, b);
    ASSERT_GT(b, a);
    ASSERT_GE(b, a);
    
    ASSERT_EQ(1990, a.year);
    ASSERT_EQ(1, a.month);
    ASSERT_EQ(10, a.day);
    ASSERT_EQ(0, a.hour);
    ASSERT_EQ(0, a.minute);
    ASSERT_EQ(13.4, a.second);

    Time c = a+400;

    ASSERT_EQ(a.year, c.year);
    ASSERT_EQ(a.month, c.month);
    ASSERT_EQ(a.day, c.day);
    ASSERT_EQ(a.hour, c.hour);
    ASSERT_NEAR(c.minute, a.minute+6, 1.0e-9);
    ASSERT_NEAR(c.second, a.second+40, 1.0e-9);

    c = a+86400*52;
    ASSERT_EQ(a.year, c.year);
    ASSERT_EQ(a.month+2, c.month);
    ASSERT_EQ(3, c.day);
    ASSERT_EQ(0, c.hour);
    ASSERT_EQ(0, c.minute);
    ASSERT_NEAR(c.second, a.second, 1.0e-9);

    c = a+60*31+40;
    ASSERT_EQ(a.year, c.year);
    ASSERT_EQ(a.month, c.month);
    ASSERT_EQ(a.day, c.day);
    ASSERT_EQ(a.hour, c.hour);
    ASSERT_EQ(a.minute+31, c.minute);
    ASSERT_NEAR(c.second, a.second+40, 1.0e-9);

    Time d;
    d = "1979-01-01 10200";

    ASSERT_EQ(1979, d.year);
    ASSERT_EQ(1, d.month);
    ASSERT_EQ(1, d.day);
    ASSERT_EQ(2, d.hour);
    ASSERT_EQ(50, d.minute);
    ASSERT_EQ(0, d.second);
}

#endif
