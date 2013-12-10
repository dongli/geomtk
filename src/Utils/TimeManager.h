#ifndef __Geomtk_TimeManager__
#define __Geomtk_TimeManager__

namespace geomtk {

struct Time {
    int years;
    int months;
    int days;
    int hours;
    int minutes;
    double seconds;
};

class TimeManager {
protected:
    bool useLeap;
    Time currTime, endTime;
    double stepSize;
    int numStep;
    int oldLevel;
    int newLevel;
public:
    TimeManager();
    ~TimeManager();

    void init();
    void advance();
    bool isFinished() const;

    double getStepSize() const;
    int getNumStep() const;
    double getSeconds() const;
    int getOldLevel() const;
    int getNewLevel() const;
};

}

#endif