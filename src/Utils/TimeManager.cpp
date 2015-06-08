#include "TimeManager.h"

namespace geomtk {

TimeManager::TimeManager() {
    _stepUnit = SECOND;
    _useLeap = false;
    _currTime.useLeap = _useLeap;
    _endTime.useLeap = _useLeap;
    _numStep = 0;
    _isInited = false;
    REPORT_ONLINE;
}

TimeManager::~TimeManager() {
    REPORT_OFFLINE;
}

void TimeManager::
init(const Time &startTime, const Time &endTime, double stepSize) {
    if (startTime > endTime) {
        REPORT_ERROR("Start time is less than end time!");
    }
    _startTime = startTime;
    _currTime = startTime;
    _endTime = endTime;
    _stepSize = stepSize;
    _isInited = true;
} // init

mark_tag tagStepSize(1), tagStepUnit(2);
sregex reStepSize = (tagStepSize= +_d) >> ' ' >> (tagStepUnit= +_w);

void TimeManager::
init(const string &startTime, const string &endTime, const string &stepSize) {
    _startTime = startTime;
    _currTime = startTime;
    _endTime = endTime;
    smatch what;
    if (regex_match(stepSize, what, reStepSize)) {
        _stepSize = atoi(what[tagStepSize].str().c_str());
        if (what[tagStepUnit] == "year" ||
            what[tagStepUnit] == "years") {
            _stepUnit = YEAR;
        } else if (what[tagStepUnit] == "month" ||
            what[tagStepUnit] == "months") {
            _stepUnit = MONTH;
        } else if (what[tagStepUnit] == "day" ||
                   what[tagStepUnit] == "days") {
            _stepUnit = DAY;
        } else if (what[tagStepUnit] == "hour" ||
                   what[tagStepUnit] == "hours"){
            _stepUnit = HOUR;
        } else if (what[tagStepUnit] == "minute" ||
                   what[tagStepUnit] == "minutes") {
            _stepUnit = MINUTE;
        } else if (what[tagStepUnit] == "second" ||
                   what[tagStepUnit] == "seconds") {
            _stepUnit = SECOND;
        } else {
            REPORT_ERROR("Unknown step unit \"" << what[tagStepUnit] << "\"!");
        }
    } else {
        REPORT_ERROR("Bad step size format: \"" << stepSize << "\"!");
    }
    _isInited = true;
} // init

void TimeManager::
reset() {
    _numStep = 0;
    _currTime = _startTime;
} // reset

void TimeManager::
reset(int numStep, const Time &currTime) {
    _numStep = numStep;
    _currTime = currTime;
} // reset

int TimeManager::
addAlarm(TimeStepUnit unit, double freq) {
    // check if there is already an alarm with same frequency
    for (uword i = 0; i < alarms.size(); ++i) {
        if (alarms[i].unit == unit && alarms[i].freq == freq) {
            return i;
        }
    }
    Alarm alarm;
    alarm.unit = unit;
    alarm.freq = freq;
    alarm.lastTime = _currTime;
    alarm.lastStep = _numStep;
    alarms.push_back(alarm);
    return alarms.size()-1;
} // addAlarm

bool TimeManager::
checkAlarm(uword i) {
#ifndef NDEBUG
    if (i > alarms.size()-1) {
        REPORT_ERROR("Alarm index \"" << i << "\" is out of range!");
    }
#endif
    double diffTime = 0.0;
    int diffStep = 0;
    switch (alarms[i].unit) {
        case YEAR:
            diffTime = _currTime.year-alarms[i].lastTime.year;
            break;
        case MONTH:
            diffTime = _currTime.month-alarms[i].lastTime.month;
            if (diffTime == -11) diffTime = 1;
            break;
        case DAY:
            diffTime = _currTime.days(alarms[i].lastTime);
            break;
        case HOUR:
            diffTime = _currTime.hours(alarms[i].lastTime);
            break;
        case MINUTE:
            diffTime = _currTime.minutes(alarms[i].lastTime);
            break;
        case STEP:
            diffStep = _numStep-alarms[i].lastStep;
            break;
        default:
            REPORT_ERROR("Under construction!");
            break;
    }
    if (alarms[i].unit == STEP) {
        if (diffStep >= alarms[i].freq || diffStep == 0) {
            alarms[i].lastStep = _numStep;
            return true;
        } else {
            return false;
        }
    }
    if (diffTime >= alarms[i].freq || diffTime == 0) {
        alarms[i].lastTime = _currTime;
        return true;
    } else {
        return false;
    }
} // checkAlarm

void TimeManager::
advance(bool mute) {
    _numStep++;
    switch (_stepUnit) {
        case YEAR:
            _currTime.year += 1;
            break;
        case MONTH:
            if (_currTime.month != 12) {
                _currTime.month += 1;
            } else {
                _currTime.month = 1;
                _currTime.year += 1;
            }
            break;
        case DAY:
            _currTime += _stepSize*TimeUnit::DAYS;
            break;
        case HOUR:
            _currTime += _stepSize*TimeUnit::HOURS;
            break;
        case MINUTE:
            _currTime += _stepSize*TimeUnit::MINUTES;
            break;
        case SECOND:
            _currTime += _stepSize;
            break;
        default:
            REPORT_ERROR("Invalid time step unit!");
    }
    if (!mute) REPORT_NOTICE(_currTime);
} // advance

int TimeManager::
totalNumStep() const {
    int res;
    switch (_stepUnit) {
        case YEAR:
            res = _endTime.year-_startTime.year+1;
            break;
        case MONTH:
            if (_endTime.year == _startTime.year) {
                res = _endTime.month-_startTime.month+1;
            } else {
                res = (_endTime.year-_startTime.year)*12-_startTime.month+1+_endTime.month;
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
} // totalNumStep

} // geomtk
