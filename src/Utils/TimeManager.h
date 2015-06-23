#ifndef __GEOMTK_TimeManager__
#define __GEOMTK_TimeManager__

#include "geomtk_commons.h"

namespace geomtk {

struct Alarm {
    variant<time_duration, days, months, years> freq;
    ptime lastTime;
    int lastStep;
}; // Alarm

class TimeManager {
protected:
    bool _useLeap;
    ptime _startTime, _currTime, _endTime;
    time_duration _stepSize;
    int _numStep;
    vector<Alarm> alarms;
    bool _isInited;
public:
    TimeManager();
    ~TimeManager();

    void
    init(const ptime &startTime, const ptime &endTime,
         const time_duration &stepSize);

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

    const time_duration&
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
