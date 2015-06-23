#include "TimeManager.h"

namespace geomtk {

TimeManager::TimeManager() {
    _useLeap = false;
    _numStep = 0;
    _isInited = false;
    REPORT_ONLINE;
}

TimeManager::~TimeManager() {
    REPORT_OFFLINE;
}

void TimeManager::
init(const ptime &startTime, const ptime &endTime,
     const time_duration &stepSize) {
    if (startTime > endTime) {
        REPORT_ERROR("Start time is less than end time!");
    }
    _startTime = startTime;
    _currTime = startTime;
    _endTime = endTime;
    _stepSize = stepSize;
    _isInited = true;
} // init

void TimeManager::
init(const ptime &startTime, const ptime &endTime,
     double stepSizeInSeconds) {
    if (startTime > endTime) {
        REPORT_ERROR("Start time is less than end time!");
    }
    _startTime = startTime;
    _currTime = startTime;
    _endTime = endTime;
    _stepSize = time_duration(0, 0, 0, time_duration::ticks_per_second()*stepSizeInSeconds);
    _isInited = true;
} // init

void TimeManager::
init(const string &startTime, const string &endTime, const string &stepSize) {
    try {
        _startTime = time_from_string(startTime);
    } catch (exception &e) {
        REPORT_ERROR("Failed to parse time string \"" << startTime << "\" with"
                     << " exception \"" << e.what() << "\"!");
    }
    try {
        _endTime = time_from_string(endTime);
    } catch (exception &e) {
        REPORT_ERROR("Failed to parse time string \"" << endTime << "\" with"
                     << " exception \"" << e.what() << "\"!");
    }
    _currTime = time_from_string(startTime);
    mark_tag tagStepSize(1), tagStepUnit(2);
    sregex reStepSize = (tagStepSize= +_d) >> ' ' >> (tagStepUnit= +_w);
    smatch what;
    if (regex_match(stepSize, what, reStepSize)) {
        double tmp = atoi(what[tagStepSize].str().c_str());
        if (what[tagStepUnit] == "hour" ||
            what[tagStepUnit] == "hours"){
            _stepSize = time_duration(0, 0, 0, tmp*3600*time_duration::ticks_per_second());
        } else if (what[tagStepUnit] == "minute" ||
                   what[tagStepUnit] == "minutes") {
            _stepSize = time_duration(0, 0, 0, tmp*60*time_duration::ticks_per_second());
        } else if (what[tagStepUnit] == "second" ||
                   what[tagStepUnit] == "seconds") {
            _stepSize = time_duration(0, 0, 0, tmp*time_duration::ticks_per_second());
        } else {
            REPORT_ERROR("Invalid step unit \"" << what[tagStepUnit] << "\"!");
        }
    } else {
        _stepSize = duration_from_string(stepSize);
    }
    _isInited = true;
} // init

void TimeManager::
reset() {
    _numStep = 0;
    _currTime = _startTime;
} // reset

void TimeManager::
reset(int numStep, const ptime &currTime) {
    _numStep = numStep;
    _currTime = currTime;
} // reset

bool TimeManager::
checkAlarm(uword i) {
#ifndef NDEBUG
    if (i > alarms.size()-1) {
        REPORT_ERROR("Alarm index \"" << i << "\" is out of range!");
    }
#endif
    if (alarms[i].freq.type() == typeid(time_duration)) {
        const auto &freq = boost::get<time_duration>(alarms[i].freq);
        if (freq.is_negative()) {
            int diff = alarms[i].lastStep-_numStep;
            if (diff == 0 || diff == freq.seconds()) {
                alarms[i].lastStep = _numStep;
                return true;
            }
        } else {
            time_duration diff = _currTime-alarms[i].lastTime;
            if (diff.total_seconds() == 0 || diff == freq) {
                alarms[i].lastTime = _currTime;
                return true;
            }
        }
    } else if (alarms[i].freq.type() == typeid(boost::gregorian::days)) {
        const auto &freq = boost::get<boost::gregorian::days>(alarms[i].freq);
        auto diffDays = _currTime.date()-alarms[i].lastTime.date();
        auto diffSeconds = _currTime.time_of_day()-alarms[i].lastTime.time_of_day();
        if (diffSeconds.total_seconds() == 0 &&
            (diffDays.days() == 0 || diffDays == freq)) {
            alarms[i].lastTime = _currTime;
            return true;
        }
    } else if (alarms[i].freq.type() == typeid(boost::gregorian::months)) {
        const auto &freq = boost::get<boost::gregorian::months>(alarms[i].freq);
        assert(freq.number_of_months() == 1);
        auto numDay = gregorian_calendar::end_of_month_day(alarms[i].lastTime.date().year(), alarms[i].lastTime.date().month());
        auto diffDays = _currTime.date()-alarms[i].lastTime.date();
        auto diffSeconds = _currTime.time_of_day()-alarms[i].lastTime.time_of_day();
        if (diffSeconds.total_seconds() == 0 &&
            (diffDays.days() == 0 || diffDays.days() == numDay)) {
            alarms[i].lastTime = _currTime;
            return true;
        }
    } else if (alarms[i].freq.type() == typeid(boost::gregorian::years)) {
        const auto &freq = boost::get<boost::gregorian::years>(alarms[i].freq);
        assert(freq.number_of_years() == 1);
        auto numDay = gregorian_calendar::is_leap_year(alarms[i].lastTime.date().year()) ? 366 : 365;
        auto diffDays = _currTime.date()-alarms[i].lastTime.date();
        auto diffSeconds = _currTime.time_of_day()-alarms[i].lastTime.time_of_day();
        if (diffSeconds.total_seconds() == 0 &&
            (diffDays.days() == 0 || diffDays.days() == numDay)) {
            alarms[i].lastTime = _currTime;
            return true;
        }
    }
    return false;
} // checkAlarm

void TimeManager::
advance(bool mute) {
    _numStep++;
    _currTime += _stepSize;
    if (!mute) REPORT_NOTICE(_currTime);
} // advance

int TimeManager::
totalNumStep() const {
    int res;
    res = static_cast<double>((_endTime-_startTime).total_seconds())/_stepSize.total_seconds();
    return res;
} // totalNumStep

} // geomtk
