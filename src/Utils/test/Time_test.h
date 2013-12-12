#ifndef __Geomtk_Time_test__
#define __Geomtk_Time_test__

#include "TimeManager.h"

using namespace geomtk;

TEST(Time, Constructor) {
    Time a;

    ASSERT_EQ(false, a.useLeap);
    ASSERT_EQ(1, a.year);
    ASSERT_EQ(1, a.month);
    ASSERT_EQ(1, a.day);
    ASSERT_EQ(0, a.hour);
    ASSERT_EQ(0, a.minute);
    ASSERT_EQ(0, a.second);
}

TEST(Time, Operations) {
    Time a;
    ASSERT_EQ(31, a.getDaysOfMonth(1));
    ASSERT_EQ(28, a.getDaysOfMonth(2));
    ASSERT_EQ(31, a.getDaysOfMonth(3));
    ASSERT_EQ(30, a.getDaysOfMonth(4));
    ASSERT_EQ(31, a.getDaysOfMonth(5));
    ASSERT_EQ(30, a.getDaysOfMonth(6));
    ASSERT_EQ(31, a.getDaysOfMonth(7));
    ASSERT_EQ(31, a.getDaysOfMonth(8));
    ASSERT_EQ(30, a.getDaysOfMonth(9));
    ASSERT_EQ(31, a.getDaysOfMonth(10));
    ASSERT_EQ(30, a.getDaysOfMonth(11));
    ASSERT_EQ(31, a.getDaysOfMonth(12));
    
    ASSERT_EQ(0, a.getDaysBeforeMonth(1));
    ASSERT_EQ(31, a.getDaysBeforeMonth(2));
    ASSERT_EQ(59, a.getDaysBeforeMonth(3));
    ASSERT_EQ(90, a.getDaysBeforeMonth(4));
    ASSERT_EQ(120, a.getDaysBeforeMonth(5));
    ASSERT_EQ(151, a.getDaysBeforeMonth(6));
    ASSERT_EQ(181, a.getDaysBeforeMonth(7));
    ASSERT_EQ(212, a.getDaysBeforeMonth(8));
    ASSERT_EQ(243, a.getDaysBeforeMonth(9));
    ASSERT_EQ(273, a.getDaysBeforeMonth(10));
    ASSERT_EQ(304, a.getDaysBeforeMonth(11));
    ASSERT_EQ(334, a.getDaysBeforeMonth(12));

    ASSERT_EQ(334, a.getDaysAfterMonth(1));
    ASSERT_EQ(306, a.getDaysAfterMonth(2));
    ASSERT_EQ(275, a.getDaysAfterMonth(3));
    ASSERT_EQ(245, a.getDaysAfterMonth(4));
    ASSERT_EQ(214, a.getDaysAfterMonth(5));
    ASSERT_EQ(184, a.getDaysAfterMonth(6));
    ASSERT_EQ(153, a.getDaysAfterMonth(7));
    ASSERT_EQ(122, a.getDaysAfterMonth(8));
    ASSERT_EQ(92, a.getDaysAfterMonth(9));
    ASSERT_EQ(61, a.getDaysAfterMonth(10));
    ASSERT_EQ(31, a.getDaysAfterMonth(11));
    ASSERT_EQ(0, a.getDaysAfterMonth(12));

    a.year = 1990;
    a.month = 1;
    a.day = 10;
    a.hour = 0;
    a.minute = 0;
    a.second = 13.4;

    Time b = a-22*86400;
    
    ASSERT_EQ(a.year-1, b.year);
    ASSERT_EQ(a.getPrevMonth(), b.month);
    ASSERT_EQ(19, b.day);
    ASSERT_EQ(0, b.hour);
    ASSERT_EQ(0, b.minute);
    ASSERT_EQ(a.second, b.second);

    ASSERT_EQ(22*86400, a.getSeconds(b));
    ASSERT_EQ(-22*86400, b.getSeconds(a));
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
    ASSERT_GT(1.0e-13, abs(a.minute+6-c.minute));
    ASSERT_GT(1.0e-13, abs(a.second+40.0-c.second));

    c = a+86400*52;
    ASSERT_EQ(a.year, c.year);
    ASSERT_EQ(a.month+2, c.month);
    ASSERT_EQ(3, c.day);
    ASSERT_EQ(0, c.hour);
    ASSERT_EQ(0, c.minute);
    ASSERT_GT(1.0e-9, abs(a.second-c.second));

    c = a+60*31+40;
    ASSERT_EQ(a.year, c.year);
    ASSERT_EQ(a.month, c.month);
    ASSERT_EQ(a.day, c.day);
    ASSERT_EQ(a.hour, c.hour);
    ASSERT_EQ(a.minute+31, c.minute);
    ASSERT_GT(1.0e-13, abs(a.second+40-c.second));
}

#endif