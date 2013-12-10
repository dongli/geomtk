#include "TimeManager.h"

namespace geomtk {

TimeManager::TimeManager() {
    currTime.years = 1; endTime.years = currTime.years;
    currTime.months = 1; endTime.months = currTime.months;
    currTime.days = 1; endTime.days = currTime.days;
    currTime.hours = 0; endTime.hours = currTime.hours;
    currTime.seconds = 0; endTime.seconds = currTime.seconds;
    useLeap = false;
    numStep = 0;
    oldLevel = 0;
    newLevel = 1;
}

TimeManager::~TimeManager() {

}

void TimeManager::init() {

}

void TimeManager::advance() {
    numStep++;
    int tmp = oldLevel;
    oldLevel = newLevel;
    newLevel = tmp;
}

bool TimeManager::isFinished() const {
    if (currTime.years == endTime.years &&
        currTime.months == endTime.months &&
        currTime.days == endTime.days &&
        currTime.hours == endTime.hours &&
        currTime.seconds == endTime.seconds) {
        return true;
    } else {
        return false;
    }
}

double TimeManager::getSeconds() const {
    double res = currTime.seconds+currTime.hours*60+(currTime.days-1)*86400;
    if (useLeap) {

    } else {
        double tmp;
        if (currTime.months == 2) {
            tmp = 31;
        } else if (currTime.months == 3) {
            tmp = 31+28;
        } else if (currTime.months == 4) {
            tmp = 31+28+31;
        } else if (currTime.months == 5) {
            tmp = 31+28+31+30;
        } else if (currTime.months == 6) {
            tmp = 31+28+31+30+31;
        } else if (currTime.months == 7) {
            tmp = 31+28+31+30+31+30;
        } else if (currTime.months == 8) {
            tmp = 31+28+31+30+31+30+31;
        } else if (currTime.months == 9) {
            tmp = 31+28+31+30+31+30+31+31;
        } else if (currTime.months == 10) {
            tmp = 31+28+31+30+31+30+31+31+30;
        } else if (currTime.months == 11) {
            tmp = 31+28+31+30+31+30+31+31+30+31;
        } else if (currTime.months == 12) {
            tmp = 31+28+31+30+31+30+31+31+30+31+30;
        }
        res += tmp*86400;
        res += (currTime.years-1)*365*86400;
    }
    return res;
}

double TimeManager::getStepSize() const {
    return stepSize;
}

int TimeManager::getNumStep() const {
    return numStep;
}

int TimeManager::getOldLevel() const {
    return oldLevel;
}

int TimeManager::getNewLevel() const {
    return newLevel;
}

}
