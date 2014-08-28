#ifndef __Geomtk_TimeManager__
#define __Geomtk_TimeManager__

#include "geomtk_commons.h"
#include "_Time.h"

namespace geomtk {

struct Alarm {
    TimeStepUnit unit;
    double freq;
    Time lastTime;
    int lastStep;
};

class TimeManager {
protected:
    bool useLeap;
    Time startTime, currTime, endTime;
    TimeStepUnit stepUnit;
    double stepSize;
    int numStep;
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
    void init(const Time &startTime, const Time &endTime, double stepSize);

    void init(const string &startTime, const string &endTime,
              const string &stepSize);

    /**
     *  Reset the current time.
     *
     *  @param time the current time.
     */
    void resetCurrentTime(Time time);

    void reset();

    /**
     *  Add an alarm with given frequency
     *
     *  @param unit the frequency unit.
     *  @param freq the frequency.
     *
     *  @return The alarm index.
     */
    int addAlarm(TimeStepUnit unit, double freq);

    /**
     *  Check if an alarm is ringing in current step.
     *
     *  @param i the alarm index.
     *
     *  @return The boolean status.
     */
    bool checkAlarm(int i);

    /**
     *  Advance the time by one time step.
     *
     *  @param mute boolean to control whether print message or not.
     */
    void advance(bool mute = false);

    /**
     *  Check if the end time is reached.
     *
     *  @return Boolean true if the end time is reached, otherwise false.
     */
    bool isFinished() const { return currTime > endTime; }
    
    const Time& getStartTime() const { return startTime; }

    const Time& getCurrTime() const { return currTime; }

    const Time& getEndTime() const { return endTime; }

    double getStepSize() const { return stepSize; }

    TimeStepUnit getStepUnit() const { return stepUnit; }

    int getNumStep() const { return numStep; }

    int getTotalNumStep() const;

    double getSeconds() const { return currTime.getSeconds(startTime); }

    double getMinutes() const { return currTime.getMinutes(startTime); }

    double getHours() const { return currTime.getHours(startTime); }

    double getDays() const { return currTime.getDays(startTime); }
};

}

#endif
