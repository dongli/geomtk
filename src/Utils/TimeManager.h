#ifndef __Geomtk_TimeManager__
#define __Geomtk_TimeManager__

#include "geomtk_commons.h"

namespace geomtk {

/**
 * Time
 * This class describes the time point.
 */

class Time {
public:
    bool useLeap;
    int year, month, day, hour, minute;
    double second;

    Time();
    virtual ~Time();

    /**
     * This method returns the seconds between this time and other time. The
     * sign is included to represent the time direction.
     */
    double getSeconds(const Time &other) const;
    
    /**
     * This method returns the days included in the given month and year. If the
     * month and year are not given, use the ones in this object.
     */
    int getDaysOfMonth(int month = -1, int year = -1) const;
    
    /**
     * This method returns the days before the given month and in the given
     * year. If the month and year are not given, use the ones in this object.
     */
    int getDaysBeforeMonth(int month = -1, int year = -1) const;
    
    /**
     * This method returns the days after the given month and in the given
     * year. If the month and year are not given, use the ones in this object.
     */
    int getDaysAfterMonth(int month = -1, int year = -1) const;
    
    /**
     * This method returns the days included in the given year. If the year is
     * not given, use the one in this object.
     */
    int getDaysOfYear(int year = -1) const;
    
    /**
     * This method returns the previous month of the given month. If the month
     * is not given, use the one in this object.
     */
    int getPrevMonth(int month = -1) const;
    
    /**
     * This method returns the next month of the given month. If the month is
     * not given, use the one in this object.
     */
    int getNextMonth(int month = -1) const;

    const Time operator+(double seconds) const;
    Time& operator+=(double seconds);
    const Time operator-(double seconds) const;
    Time& operator=(const Time &other);
    bool operator==(const Time &other) const;
    bool operator>(const Time &other) const;
    bool operator>=(const Time &other) const;
    bool operator<(const Time &other) const;
    bool operator<=(const Time &other) const;

    string s(bool onlyDate = false) const;

    friend inline ostream& operator<<(ostream &os, Time &other) {
        os << other.s();
        return os;
    }
};

class TimeManager {
protected:
    bool useLeap;
    Time startTime, currTime, endTime;
    double stepSize;
    int numStep;
public:
    TimeManager();
    ~TimeManager();

    void init(Time startTime, Time endTime, double stepSize);
    void advance();
    bool isFinished() const;

    const Time& getCurrTime() const;
    double getStepSize() const;
    int getNumStep() const;
    double getSeconds() const;
};

}

#endif