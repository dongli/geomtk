#include "_Time.h"

namespace geomtk {

Time::
Time() {
    reset();
}

Time::
Time(double seconds) {
    reset();
    *this += seconds;
}

Time::
~Time() {
}

void Time::
reset() {
    useLeap = false;
    year = 0;
    month = 0;
    day = 0;
    hour = 0;
    minute = 0;
    second = 0;
} // reset

double Time::
tod() const {
    return hour*TimeUnit::HOURS+minute*TimeUnit::MINUTES+second;
} // tod

double Time::
seconds(const Time &other) const {
    double res = 0; int sign = 1;
    const Time *big = this;
    const Time *small = &other;
    // judge the sign and set the operands
    if (year == other.year) {
        if (month == other.month) {
            if (day == other.day) {
                if (hour == other.hour) {
                    if (minute == other.minute) {
                        if (second < other.second) {
                            big = &other; small = this; sign = -1;
                        }
                    } else if (minute < other.minute) {
                        big = &other; small = this; sign = -1;
                    }
                } else if (hour < other.hour) {
                    big = &other; small = this; sign = -1;
                }
            } else if (day < other.day) {
                big = &other; small = this; sign = -1;
            }
        } else if (month < other.month) {
            big = &other; small = this; sign = -1;
        }
    } else if (year < other.year) {
        big = &other; small = this; sign = -1;
    }
    for (int i = small->year+1; i < big->year; ++i) {
        res += daysOfYear(i)*86400;
    }
    bool flag = small->year != big->year;
    if (flag) {
        for (int i = small->month+1; i <= 12; ++i) {
            res += small->daysOfMonth(i)*86400;
        }
        for (int i = 1; i < big->month; ++i) {
            res += big->daysOfMonth(i)*86400;
        }
    } else {
        for (int i = small->month+1; i < big->month; ++i) {
            res += small->daysOfMonth(i)*86400;
        }
    }
    flag = flag || small->month != big->month;
    if (flag) {
        res += (small->daysOfMonth(small->month)-small->day)*86400;
        res += (big->day-1)*86400;
    } else {
        if (small->day < big->day) {
            res += (big->day-small->day-1)*86400;
        }
    }
    flag = flag || small->day != big->day;
    if (flag) {
        res += (24-small->hour-1)*3600;
        res += big->hour*3600;
    } else {
        if (small->hour < big->hour) {
            res += (big->hour-small->hour-1)*3600;
        }
    }
    flag = flag || small->hour != big->hour;
    if (flag) {
        res += (60-small->minute-1)*60;
        res += big->minute*60;
    } else {
        if (small->minute < big->minute) {
            res += (big->minute-small->minute-1)*60;
        }
    }
    flag = flag || small->minute != big->minute;
    if (flag) {
        res += 60-small->second;
        res += big->second;
    } else {
        res += big->second-small->second;
    }
    return res*sign;
} // seconds

double Time::
minutes(const Time &other) const {
    return seconds(other)/60;
} // minutes

double Time::
hours(const Time &other) const {
    return seconds(other)/3600;
} // hours

double Time::
days(const Time &other) const {
    return seconds(other)/86400;
} // days

int Time::
daysOfMonth(int month, int year) const {
    if (month == -1) {
        month = this->month;
    }
    if (useLeap) {
        REPORT_ERROR("Under construction!");
        if (year == -1) {
            year = this->year;
        }
    } else {
        switch (month) {
            case 1: case 3: case 5: case 7: case 8: case 10: case 12:
                return 31;
            case 4: case 6: case 9: case 11:
                return 30;
            case 2:
                return 28;
            default:
                REPORT_ERROR("Wrong month " << month << "!");
        }
    }
} // daysOfMonth
    
int Time::
daysBeforeMonth(int month, int year) const {
    if (month == -1) {
        month = this->month;
    }
    int res = 0;
    for (int i = 1; i < month; ++i) {
        res += daysOfMonth(i, year);
    }
    return res;
} // daysBeforeMonth
    
int Time::
daysAfterMonth(int month, int year) const {
    if (month == -1) {
        month = this->month;
    }
    int res = 0;
    for (int i = month+1; i <= 12; ++i) {
        res += daysOfMonth(i, year);
    }
    return res;
} // daysAfterMonth

int Time::
daysOfYear(int year) const {
    if (useLeap) {
        REPORT_ERROR("Under construction!");
        if (year == -1) {
            year = this->year;
        }
    } else {
        return 365;
    }
} // daysOfYear

int Time::
prevMonth(int month) const {
    if (month == -1) {
        month = this->month;
    }
    return month == 1 ? 12 : month-1;
} // prevMonth
    
int Time::
nextMonth(int month) const {
    if (month == -1) {
        month = this->month;
    }
    return month == 12 ? 1 : month+1;
} // nextMonth
    
const Time Time::
operator+(double seconds) const {
    Time res = *this;
    double remain = seconds; // remained time (unit will be changed)
    // handle second
    if (remain < 60-res.second) {
        res.second += remain;
        return res;
    } else {
        remain += res.second;
        res.second = fmod(remain, 60);
        remain -= res.second;
    }
    // handle minute
    remain /= 60; // turn into minutes
    if (remain < 60-res.minute) {
        res.minute += remain;
        return res;
    } else {
        remain += res.minute;
        res.minute = int(remain)%60;
        remain -= res.minute;
    }
    // handle hour
    remain /= 60; // turn into hours
    if (remain < 24-res.hour) {
        res.hour += remain;
        return res;
    } else {
        remain += res.hour;
        res.hour = int(remain)%24;
        remain -= res.hour;
    }
    // handle day
    remain /= 24; // turn into days
    while (true) {
        if (remain <= daysOfMonth(res.month, res.year)-res.day) {
            res.day += remain;
            return res;
        }
        remain -= daysOfMonth(res.month, res.year);
        res.month++;
        if (res.month == 13) {
            res.month = 1;
            res.year++;
        }
    }
    return res;
} // operator+
    
Time& Time::
operator+=(double seconds) {
    *this = *this+seconds;
    return *this;
} // operator+=

const Time Time::
operator-(double seconds) const {
    Time res = *this;
    double remain = seconds; // remained time (unit will be changed)
    // handle second
    if (remain <= res.second) {
        // second is enough
        res.second -= remain;
        return res;
    } else {
        // second is not enough
        double tmp = fmod(remain, 60);
        if (tmp <= res.second) {
            res.second -= tmp;
            remain -= tmp;
        } else {
            // borrow one minute and record it in 'remain'
            res.second += 60-tmp;
            remain += 60-tmp;
        }
    }
    // handle minute
    remain /= 60; // turn into minutes
    if (remain <= res.minute) {
        // minute is enough
        res.minute -= remain;
        return res;
    } else {
        // minute is not enough
        int tmp = int(remain)%60;
        if (tmp <= res.minute) {
            res.minute -= tmp;
            remain -= tmp;
        } else {
            // borrow one hour and record it in 'remain'
            res.minute += 60-tmp;
            remain += 60-tmp;
        }
    }
    // handle hour
    remain /= 60; // turn into hours
    if (remain <= res.hour) {
        // hour is enough
        res.hour -= remain;
        return res;
    } else {
        // hour is not enough
        int tmp = int(remain)%24;
        if (tmp <= res.hour) {
            res.hour -= tmp;
            remain -= tmp;
        } else {
            // borrow one day and record it in 'remain'
            res.hour += 24-tmp;
            remain += 24-tmp;
        }
    }
    // handle day
    remain /= 24; // turn into days
    while (true) {
        if (remain <= res.day) {
            // day is enough
            res.day -= remain;
            return res;
        }
        if (res.month == 1) {
            // day is not enough and borrow one month
            res.day += daysOfMonth(res.month, res.year);
            res.month = 12;
            res.year--;
        }
    }
    return res;
} // operator-

Time& Time::
operator=(const Time &other) {
    if (this != &other) {
        year = other.year;
        month = other.month;
        day = other.day;
        hour = other.hour;
        minute = other.minute;
        second = other.second;
    }
    return *this;
} // operator=

static mark_tag tagYear(1), tagMonth(2), tagDay(3), tagTod(4);
static sregex reTime = (tagYear= repeat<4, 4>(_d)) >> '-' >>
                       (tagMonth= repeat<2, 2>(_d)) >>
                       optional('-' >> (tagDay= repeat<2, 2>(_d))) >>
                       optional(' ' >> (tagTod= repeat<5, 5>(_d)));

Time& Time::
operator=(const string &other) {
    smatch what;
    if (regex_search(other, what, reTime)) {
        year = stoi(what[tagYear].str());
        month = stoi(what[tagMonth].str());
        if (what[tagDay] != "") {
            day = stoi(what[tagDay].str());
        }
        if (what[tagTod].str() != "") {
            int tod = atoi(what[tagTod].str().c_str());
            hour = tod/TimeUnit::HOURS;
            tod -= hour*TimeUnit::HOURS;
            minute = tod/TimeUnit::MINUTES;
            tod -= minute*TimeUnit::MINUTES;
            second = tod;
        }
    } else {
        REPORT_ERROR("Bad time string format: \"" << other << "\"!");
    }
    return *this;
} // operator=

Time& Time::
operator=(const char *other) {
    string other_(other);
    return operator=(other_);
} // operator=

bool Time::
operator==(const Time &other) const {
    static const double eps = 1.0e-12;
    if (year == other.year && month == other.month &&
        day == other.day && hour == other.hour &&
        minute == other.minute && fabs(second-other.second) < eps) {
        return true;
    } else {
        return false;
    }
} // operator==

bool Time::
operator>(const Time &other) const {
    static Time epoch;
    epoch.year = 1970;
    epoch.month = 1;
    epoch.day = 1;
    if (this->seconds(epoch) > other.seconds(epoch)) {
        return true;
    } else {
        return false;
    }
} // operator>

bool Time::operator>=(const Time &other) const {
    static Time epoch;
    epoch.year = 1970;
    if (this->seconds(epoch) >= other.seconds(epoch)) {
        return true;
    } else {
        return false;
    }
}

bool Time::operator<(const Time &other) const {
    return !Time::operator>=(other);
}

bool Time::operator<=(const Time &other) const {
    return !Time::operator>(other);
}

string Time::s(bool onlyDate) const {
    char tmp[100];
    if (onlyDate) {
        sprintf(tmp, "%4.4d-%2.2d-%2.2d", year, month, day);
    } else {
        if (static_cast<int>(second) == second) {
            sprintf(tmp, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d",
                    year, month, day, hour, minute, static_cast<int>(second));
        } else {
            sprintf(tmp, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%f",
                    year, month, day, hour, minute, second);
        }
    }
    string res = tmp;
    return res;
}

} // geomtk
