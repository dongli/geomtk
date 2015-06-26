#include "TimeManager.h"

namespace geomtk {

TimeManager::TimeManager() {
    _numStep = 0;
    _isInited = false;
    cout.imbue(std::locale(cout.getloc(), new time_facet("%Y-%m-%d %H:%M:%s")));
    REPORT_ONLINE;
}

TimeManager::~TimeManager() {
    REPORT_OFFLINE;
}

void TimeManager::
init(const ptime &startTime, const ptime &endTime,
     double stepSizeInSeconds) {
    if (startTime > endTime) {
        REPORT_ERROR("Start time is less than end time!");
    }
    _startTime = startTime;
    _currTime = startTime;
    _endTime = endTime;
    _stepSize = geomtk::seconds(stepSizeInSeconds);
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
    _stepSize = durationFromString(stepSize);
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

double TimeManager::
stepSizeInSeconds() const {
    double res;
    if (_stepSize.type() == typeid(time_duration)) {
        res = boost::get<time_duration>(_stepSize).total_seconds();
    } else if (_stepSize.type() == typeid(boost::gregorian::days)) {
        res = boost::get<boost::gregorian::days>(_stepSize).days()*86400;
    } else if (_stepSize.type() == typeid(boost::gregorian::months)) {
        REPORT_ERROR("Step size unit is month and cannot be converted to seconds!");
    } else if (_stepSize.type() == typeid(boost::gregorian::years)) {
        REPORT_ERROR("Step size unit is year and cannot be converted to seconds!");
    }
    return res;
} // stepSizeInSeconds

void TimeManager::
advance(bool mute) {
    _numStep++;
    if (_stepSize.type() == typeid(time_duration)) {
        _currTime += boost::get<time_duration>(_stepSize);
    } else if (_stepSize.type() == typeid(boost::gregorian::days)) {
        _currTime += boost::get<boost::gregorian::days>(_stepSize);
    } else if (_stepSize.type() == typeid(boost::gregorian::months)) {
        _currTime += boost::get<boost::gregorian::months>(_stepSize);
    } else if (_stepSize.type() == typeid(boost::gregorian::years)) {
        _currTime += boost::get<boost::gregorian::years>(_stepSize);
    }
    if (!mute) REPORT_NOTICE(_currTime);
} // advance

int TimeManager::
totalNumStep() const {
    int res;
    if (_stepSize.type() == typeid(time_duration)) {
        res = static_cast<double>((_endTime-_startTime).total_seconds())/boost::get<time_duration>(_stepSize).total_seconds();
    } else if (_stepSize.type() == typeid(boost::gregorian::days)) {
        res = static_cast<double>((_endTime.date()-_startTime.date()).days()/boost::get<boost::gregorian::days>(_stepSize).days());
    } else if (_stepSize.type() == typeid(boost::gregorian::months)) {
        res = (_endTime.date().year()-_startTime.date().year())*12+_endTime.date().month()-_startTime.date().month();
    } else if (_stepSize.type() == typeid(boost::gregorian::years)) {
        res = (_endTime.date().year()-_startTime.date().year()+1)/boost::get<boost::gregorian::years>(_stepSize).number_of_years().as_number();
    }
    return res;
} // totalNumStep

} // geomtk
