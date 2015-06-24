#ifndef __GEOMTK_TimeManager__
#define __GEOMTK_TimeManager__

#include "geomtk_commons.h"

namespace geomtk {

typedef variant<time_duration, days, months, years> duration;

static duration
durationFromString(const string &str) {
    duration res;
    mark_tag tagSize(1), tagUnit(2);
    sregex reDuration = (tagSize= +_d) >> ' ' >> (tagUnit= +_w);
    smatch what;
    if (regex_match(str, what, reDuration)) {
        double tmp = atoi(what[tagSize].str().c_str());
        if (what[tagUnit] == "hour" ||
            what[tagUnit] == "hours"){
            res = time_duration(0, 0, 0, tmp*3600*time_duration::ticks_per_second());
        } else if (what[tagUnit] == "minute" ||
                   what[tagUnit] == "minutes") {
            res = time_duration(0, 0, 0, tmp*60*time_duration::ticks_per_second());
        } else if (what[tagUnit] == "second" ||
                   what[tagUnit] == "seconds") {
            res = time_duration(0, 0, 0, tmp*time_duration::ticks_per_second());
        } else if (what[tagUnit] == "day" ||
                   what[tagUnit] == "days") {
            res = days(tmp);
        } else if (what[tagUnit] == "month" ||
                   what[tagUnit] == "months") {
            res = months(tmp);
        } else if (what[tagUnit] == "year" ||
                   what[tagUnit] == "years") {
            res = years(tmp);
        } else {
            REPORT_ERROR("Invalid step unit \"" << what[tagUnit] << "\"!");
        }
    } else {
        res = duration_from_string(str);
    }
    return res;
} // timeDurationFromString

struct Alarm {
    duration freq;
    ptime lastTime;
    int lastStep;
}; // Alarm

class TimeManager {
protected:
    bool _useLeap;
    ptime _startTime, _currTime, _endTime;
    duration _stepSize;
    int _numStep;
    vector<Alarm> alarms;
    bool _isInited;
public:
    TimeManager();
    ~TimeManager();

    template <class DurationType>
    void
    init(const ptime &startTime, const ptime &endTime,
         const DurationType &stepSize);

    void
    init(const ptime &startTime, const ptime &endTime,
         double stepSizeInSeconds);

    void
    init(const string &startTime, const string &endTime,
         const string &stepSize);

    bool
    isInited() const {
        return _isInited;
    }

    void
    reset();

    void
    reset(int numStep, const ptime &time);

    template <class FreqType>
    int
    addAlarm(const FreqType &freq);

    bool
    checkAlarm(uword i);

    const Alarm&
    alarm(uword i) const {
        return alarms[i];
    }

    void
    advance(bool mute = false);

    bool
    isFinished() const {
        return _currTime > _endTime;
    }
    
    const ptime&
    startTime() const {
        return _startTime;
    }

    const ptime&
    currTime() const {
        return _currTime;
    }

    const ptime&
    endTime() const {
        return _endTime;
    }

    double
    stepSizeInSeconds() const;

    const duration&
    stepSize() const {
        return _stepSize;
    }

    int
    numStep() const {
        return _numStep;
    }

    int
    totalNumStep() const;

    double
    seconds() const {
        return (_currTime-_startTime).total_seconds();
    }

    double
    minutes() const {
        return seconds()/60;
    }

    double
    hours() const {
        return minutes()/60;
    }

    double
    days() const {
        return hours()/24;
    }
}; // TimeManager

template <class _DurationType>
void TimeManager::
init(const ptime &startTime, const ptime &endTime,
     const _DurationType &stepSize) {
    if (startTime > endTime) {
        REPORT_ERROR("Start time is less than end time!");
    }
    _startTime = startTime;
    _currTime = startTime;
    _endTime = endTime;
    _stepSize = stepSize;
    _isInited = true;
} // init

template <class FreqType>
int TimeManager::
addAlarm(const FreqType &freq) {
    for (uword i = 0; i < alarms.size(); ++i) {
        if (alarms[i].freq.type() == typeid(FreqType) &&
            boost::get<FreqType>(alarms[i].freq) == freq) {
            return i;
        }
    }
    Alarm alarm;
    alarm.freq = freq;
    alarm.lastTime = _currTime;
    alarm.lastStep = _numStep;
    alarms.push_back(alarm);
    return alarms.size()-1;
} // addAlarm

} // geomtk

#endif // __GEOMTK_TimeManager__
