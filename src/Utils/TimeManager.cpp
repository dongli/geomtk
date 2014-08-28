#include "TimeManager.h"

namespace geomtk {

TimeManager::TimeManager() {
    stepUnit = SECOND;
    useLeap = false;
    currTime.useLeap = useLeap;
    endTime.useLeap = useLeap;
    numStep = 0;
}

TimeManager::~TimeManager() {

}

void TimeManager::init(const Time &startTime, const Time &endTime,
                       double stepSize) {
    if (startTime > endTime) {
        REPORT_ERROR("Start time is less than end time!");
    }
    this->startTime = startTime;
    currTime = startTime;
    this->endTime = endTime;
    this->stepSize = stepSize;
}

mark_tag tagStepSize(1), tagStepUnit(2);
sregex reStepSize = (tagStepSize= +_d) >> ' ' >> (tagStepUnit= +_w);

void TimeManager::init(const string &startTime, const string &endTime,
                       const string &stepSize) {
    this->startTime = startTime;
    currTime = this->startTime;
    this->endTime = endTime;
    smatch what;
    if (regex_match(stepSize, what, reStepSize)) {
        this->stepSize = atoi(what[tagStepSize].str().c_str());
        if (what[tagStepUnit] == "year" ||
            what[tagStepUnit] == "years") {
            stepUnit = YEAR;
        } else if (what[tagStepUnit] == "month" ||
            what[tagStepUnit] == "months") {
            stepUnit = MONTH;
        } else if (what[tagStepUnit] == "day" ||
                   what[tagStepUnit] == "days") {
            stepUnit = DAY;
        } else if (what[tagStepUnit] == "hour" ||
                   what[tagStepUnit] == "hours"){
            stepUnit = HOUR;
        } else if (what[tagStepUnit] == "minute" ||
                   what[tagStepUnit] == "minutes") {
            stepUnit = MINUTE;
        } else if (what[tagStepUnit] == "second" ||
                   what[tagStepUnit] == "seconds") {
            stepUnit = SECOND;
        } else {
            REPORT_ERROR("Unknown step unit \"" << what[tagStepUnit] << "\"!");
        }
    } else {
        REPORT_ERROR("Bad step size format: \"" << stepSize << "\"!");
    }
}

void TimeManager::resetCurrentTime(Time time) {
    currTime = time;
}

void TimeManager::reset() {
    numStep = 0;
    currTime = startTime;
}

int TimeManager::addAlarm(TimeStepUnit unit, double freq) {
    // check if there is already an alarm with same frequency
    for (int i = 0; i < alarms.size(); ++i) {
        if (alarms[i].unit == unit && alarms[i].freq == freq) {
            REPORT_WARNING("There is already an alarm with frequency " <<
                           freq << " " << unit << "!");
            return i;
        }
    }
    Alarm alarm;
    alarm.unit = unit;
    alarm.freq = freq;
    alarm.lastTime = currTime;
    alarm.lastStep = numStep;
    alarms.push_back(alarm);
    return alarms.size()-1;
}

bool TimeManager::checkAlarm(int i) {
#ifndef NDEBUG
    if (i < 0 || i > alarms.size()-1) {
        REPORT_ERROR("Alarm index \"" << i << "\" is out of range!");
    }
#endif
    double diffTime = 0.0;
    int diffStep = 0;
    switch (alarms[i].unit) {
        case YEAR:
            diffTime = currTime.year-alarms[i].lastTime.year;
            break;
        case MONTH:
            diffTime = currTime.month-alarms[i].lastTime.month;
            if (diffTime == -11) diffTime = 1;
            break;
        case DAY:
            diffTime = currTime.getDays(alarms[i].lastTime);
            break;
        case HOUR:
            diffTime = currTime.getHours(alarms[i].lastTime);
            break;
        case MINUTE:
            diffTime = currTime.getMinutes(alarms[i].lastTime);
            break;
        case STEP:
            diffStep = numStep-alarms[i].lastStep;
            break;
        default:
            REPORT_ERROR("Under construction!");
            break;
    }
    if (alarms[i].unit == STEP) {
        if (diffStep == alarms[i].freq || diffStep == 0) {
            alarms[i].lastStep = numStep;
            return true;
        } else {
            return false;
        }
    }
    if (diffTime >= alarms[i].freq || diffTime == 0) {
        alarms[i].lastTime = currTime;
        return true;
    } else {
        return false;
    }
}

void TimeManager::advance(bool mute) {
    numStep++;
    switch (stepUnit) {
        case YEAR:
            currTime.year += 1;
            break;
        case MONTH:
            if (currTime.month != 12) {
                currTime.month += 1;
            } else {
                currTime.month = 1;
                currTime.year += 1;
            }
            break;
        case DAY:
            currTime += stepSize*TimeUnit::DAYS;
            break;
        case HOUR:
            currTime += stepSize*TimeUnit::HOURS;
            break;
        case MINUTE:
            currTime += stepSize*TimeUnit::MINUTES;
            break;
        case SECOND:
            currTime += stepSize;
            break;
        default:
            REPORT_ERROR("Invalid time step unit!");
    }
    if (!mute) REPORT_NOTICE(currTime);
}

int TimeManager::getTotalNumStep() const {
    int res;
    switch (stepUnit) {
        case YEAR:
            res = endTime.year-startTime.year+1;
            break;
        case MONTH:
            if (endTime.year == startTime.year) {
                res = endTime.month-startTime.month+1;
            } else {
                res = (endTime.year-startTime.year)*12-startTime.month+1+endTime.month;
            }
            break;
        case DAY:
            REPORT_ERROR("Under construction!");
            break;
        case HOUR:
            REPORT_ERROR("Under construction!");
            break;
        case MINUTE:
            REPORT_ERROR("Under construction!");
            break;
        case SECOND:
            REPORT_ERROR("Under construction!");
            break;
        default:
            REPORT_ERROR("Invalid time step unit!");
    }
    return res;
}

}
