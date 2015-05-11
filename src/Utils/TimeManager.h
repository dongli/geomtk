#ifndef __GEOMTK_TimeManager__
#define __GEOMTK_TimeManager__

#include "geomtk_commons.h"
#include "_Time.h"

namespace geomtk {

struct Alarm {
    TimeStepUnit unit;
    double freq;
    Time lastTime;
    int lastStep;
}; // Alarm

class TimeManager {
protected:
    bool _useLeap;
    Time _startTime, _currTime, _endTime;
    TimeStepUnit _stepUnit;
    double _stepSize;
    int _numStep;
    vector<Alarm> alarms;
public:
    TimeManager();
    ~TimeManager();

    /**
     *  Initialize time manager.
     *
     *  @param startTime the start time.
     *  @param endTime   the end time.
     *  @param stepSize  the time step size in seconds.
     */
    void
    init(const Time &startTime, const Time &endTime, double stepSize);

    void
    init(const string &startTime, const string &endTime,
              const string &stepSize);

    void
    reset();

    void
    reset(int numStep, const Time &time);

    /**
     *  Add an alarm with given frequency
     *
     *  @param unit the frequency unit.
     *  @param freq the frequency.
     *
     *  @return The alarm index.
     */
    int
    addAlarm(TimeStepUnit unit, double freq);

    /**
     *  Check if an alarm is ringing in current step.
     *
     *  @param i the alarm index.
     *
     *  @return The boolean status.
     */
    bool
    checkAlarm(uword i);

    /**
     *  Advance the time by one time step.
     *
     *  @param mute boolean to control whether print message or not.
     */
    void
    advance(bool mute = false);

    /**
     *  Check if the end time is reached.
     *
     *  @return Boolean true if the end time is reached, otherwise false.
     */
    bool
    isFinished() const {
        return _currTime > _endTime;
    }
    
    const Time&
    startTime() const {
        return _startTime;
    }

    const Time&
    currTime() const {
        return _currTime;
    }

    const Time&
    endTime() const {
        return _endTime;
    }

    double
    stepSize() const {
        return _stepSize;
    }

    TimeStepUnit
    stepUnit() const {
        return _stepUnit;
    }

    int
    numStep() const {
        return _numStep;
    }

    int
    totalNumStep() const;

    double
    seconds() const {
        return _currTime.seconds(_startTime);
    }

    double
    minutes() const {
        return _currTime.minutes(_startTime);
    }

    double
    hours() const {
        return _currTime.hours(_startTime);
    }

    double
    days() const {
        return _currTime.days(_startTime);
    }
}; // TimeManager

} // geomtk

#endif // __GEOMTK_TimeManager__
