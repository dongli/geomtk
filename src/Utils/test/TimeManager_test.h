#ifndef __GEOMTK_TimeManager_test__
#define __GEOMTK_TimeManager_test__

#include "TimeManager.h"

using namespace geomtk;

TEST(TimeManager, Init) {
    TimeManager timeManager;
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    EXPECT_DEATH(timeManager.init("2012-01-02", "2015-02-19 00:00:00", "24 hours"),
                 "Failed to parse time string \"2012-01-02\"");
    timeManager.init("2012-01-02 00:00:00", "2015-02-19 00:00:00", "1 day");
    ASSERT_EQ(ptime(date(2012, 1, 2), seconds(0)), timeManager.startTime());
    ASSERT_EQ(ptime(date(2015, 2, 19), seconds(0)), timeManager.endTime());
    ASSERT_EQ(timeManager.currTime(), timeManager.startTime());
    ASSERT_EQ(days(1), boost::get<days>(timeManager.stepSize()));
    timeManager.init("2012-01-02 00:00:00", "2012-02-19 00:00:00", "24:05:00");
    ASSERT_EQ(ptime(date(2012, 1, 2), seconds(0)), timeManager.startTime());
    ASSERT_EQ(ptime(date(2012, 2, 19), seconds(0)), timeManager.endTime());
    ASSERT_EQ(timeManager.currTime(), timeManager.startTime());
    ASSERT_EQ(time_duration(24, 5, 0).total_seconds(), timeManager.stepSizeInSeconds());
}

TEST(TimeManager, Advance) {
    TimeManager timeManager;
    timeManager.init("2012-01-02 00:00:00", "2012-01-4 00:00:00", "24 hours");
    timeManager.advance(true);
    ASSERT_FALSE(timeManager.isFinished());
    ASSERT_EQ(timeManager.startTime()+boost::get<time_duration>(timeManager.stepSize()), timeManager.currTime());
    timeManager.advance(true);
    ASSERT_EQ(timeManager.endTime(), timeManager.currTime());
    ASSERT_FALSE(timeManager.isFinished());
    timeManager.advance(true);
    ASSERT_LT(timeManager.endTime(), timeManager.currTime());
    ASSERT_TRUE(timeManager.isFinished());
    ASSERT_EQ(2, timeManager.totalNumStep());
}

TEST(TimeManager, Seconds) {
    TimeManager timeManager;
    double dt = 1.54325;
    timeManager.init("2012-01-01 00:00:00", "2012-01-1 00:01:00", dt);
    double seconds = 0;
    while (!timeManager.isFinished()) {
        seconds += dt;
        timeManager.advance(true);
        ASSERT_NEAR(seconds, timeManager.seconds(), 1.0e-12);
    }
}

TEST(TimeManager, Alarm) {
    TimeManager timeManager;
    timeManager.init("2012-01-02 00:00:00", "2013-12-02 00:00:00", "3600 seconds");
    int a1 = timeManager.addAlarm(seconds(3600));
    int a2 = timeManager.addAlarm(minutes(120));
    int a3 = timeManager.addAlarm(days(1));
    int a4 = timeManager.addAlarm(months(1));
    int a5 = timeManager.addAlarm(years(1));
    while (!timeManager.isFinished()) {
        double seconds = timeManager.seconds();
        if (fmod(seconds, 3600) == 0) {
            ASSERT_TRUE(timeManager.checkAlarm(a1));
        } else {
            ASSERT_FALSE(timeManager.checkAlarm(a1));
        }
        if (fmod(seconds, 120*60) == 0) {
            ASSERT_TRUE(timeManager.checkAlarm(a2));
        } else {
            ASSERT_FALSE(timeManager.checkAlarm(a2));
        }
        if (fmod(seconds, 86400) == 0) {
            ASSERT_TRUE(timeManager.checkAlarm(a3));
        } else {
            ASSERT_FALSE(timeManager.checkAlarm(a3));
        }
        if (timeManager.checkAlarm(a4)) {
            ASSERT_EQ(2, timeManager.currTime().date().day());
            ASSERT_EQ(time_duration(0, 0, 0), timeManager.currTime().time_of_day());
        }
        if (timeManager.checkAlarm(a5)) {
            ASSERT_EQ(1, timeManager.currTime().date().month());
            ASSERT_EQ(2, timeManager.currTime().date().day());
            ASSERT_EQ(time_duration(0, 0, 0), timeManager.currTime().time_of_day());
        }
        timeManager.advance(true);
    }
}

TEST(TimeManager, TotalNumStep) {
    TimeManager timeManager;
    timeManager.init("2000-01-01 00:00:00", "2001-05-15 00:00:00", "1 month");
    ASSERT_EQ(16, timeManager.totalNumStep());
}

#endif // __GEOMTK_TimeManager_test__
