#ifndef __Geomtk_Time__
#define __Geomtk_Time__

#include "geomtk_commons.h"

namespace geomtk {

struct TimeUnit {
    static const int SECONDS = 1;
    static const int MINUTES = 60;
    static const int HOURS = 3600;
    static const int DAYS = 86400;
};

enum TimeStepUnit {
    YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, STEP
};

/**
 *  This class describes the time point.
 */
class Time {
public:
    bool useLeap;
    int year, month, day, hour, minute;
    double second;

    Time();
    Time(double seconds);
    virtual ~Time();

    virtual void reset();

    double getTOD() const;

    /**
     *  This method returns the seconds between this time and other time. The
     *  sign is included to represent the time direction.
     */
    double getSeconds(const Time &other) const;

    double getMinutes(const Time &other) const;
    
    double getHours(const Time &other) const;
    
    double getDays(const Time &other) const;

    double getMonths(const Time &other) const;

    /**
     *  This method returns the days included in the given month and year. If
     *  the month and year are not given, use the ones in this object.
     */
    int getDaysOfMonth(int month = -1, int year = -1) const;
    
    /**
     *  This method returns the days before the given month and in the given
     *  year. If the month and year are not given, use the ones in this object.
     */
    int getDaysBeforeMonth(int month = -1, int year = -1) const;
    
    /**
     *  This method returns the days after the given month and in the given
     *  year. If the month and year are not given, use the ones in this object.
     */
    int getDaysAfterMonth(int month = -1, int year = -1) const;
    
    /**
     *  This method returns the days included in the given year. If the year is
     *  not given, use the one in this object.
     */
    int getDaysOfYear(int year = -1) const;
    
    /**
     *  This method returns the previous month of the given month. If the month
     *  is not given, use the one in this object.
     */
    int getPrevMonth(int month = -1) const;
    
    /**
     *  This method returns the next month of the given month. If the month is
     *  not given, use the one in this object.
     */
    int getNextMonth(int month = -1) const;

    const Time operator+(double seconds) const;

    Time& operator+=(double seconds);

    const Time operator-(double seconds) const;

    Time& operator=(const Time &other);

    Time& operator=(const string &other);

    Time& operator=(const char *other);

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

} // geomtk

#endif // __Geomtk_Time__
