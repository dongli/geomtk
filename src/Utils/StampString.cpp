#include "StampString.h"

namespace geomtk {

StampString::StampString() {
    prefix = "";
    suffix = "";
    pattern = "";
    re4DigitYear = sregex::compile("%Y");
    re2DigitYear = sregex::compile("%y");
    re2DigitMonth = sregex::compile("%M");
    reMonth = sregex::compile("%m");
    re2DigitDay = sregex::compile("%D");
    reDay = sregex::compile("%d");
    re5DigitTOD = sregex::compile("%T");
    reTOD = sregex::compile("%t");
}

StampString::StampString(const string &prefix, const string &suffix) {
    this->prefix = prefix;
    this->suffix = suffix;
}

StampString::StampString(const string &pattern) {
    this->pattern = pattern;
}

StampString::~StampString() {
}

string StampString::run(const Time &time) {
    return prefix+time.s(true)+suffix;
}

string StampString::run(const string &format, int tag) {
    char buffer[1000];
    sprintf(buffer, format.c_str(), tag);
    return prefix+string(buffer)+suffix;
}

// test.gamil.h0.%Y-%M.nc
string StampString::run(const TimeManager &timeManager) {
    string res = pattern;
    smatch what;
    const Time &currTime = timeManager.getCurrTime();

    // check year
    if (regex_search(pattern, what, re4DigitYear)) {
        stringstream ss; ss << currTime.year;
        res = regex_replace(res, re4DigitYear, ss.str());
    } else if (regex_search(pattern, what, re2DigitYear)) {
        stringstream ss; ss << fmod(currTime.year, 100);
        res = regex_replace(res, re2DigitYear, ss.str());
    }

    // check month
    if (regex_search(pattern, what, re2DigitMonth)) {
        stringstream ss; ss << setw(2) << setfill('0') << currTime.month;
        res = regex_replace(res, re2DigitMonth, ss.str());
    } else if (regex_search(pattern, what, reMonth)) {
        stringstream ss; ss << currTime.month;
        res = regex_replace(res, reMonth, ss.str());
    }

    // check day
    if (regex_search(pattern, what, re2DigitDay)) {
        stringstream ss; ss << setw(2) << setfill('0') << currTime.day;
        res = regex_replace(res, re2DigitDay, ss.str());
    } else if (regex_search(pattern, what, reDay)) {
        stringstream ss; ss << currTime.day;
        res = regex_replace(res, reDay, ss.str());
    }

    // check TOD
    if (regex_search(pattern, what, re5DigitTOD)) {
        stringstream ss; ss << setw(5) << setfill('0') << currTime.getTOD();
        res = regex_replace(res, re5DigitTOD, ss.str());
    } else if (regex_search(pattern, what, reTOD)) {
        stringstream ss; ss << currTime.getTOD();
        res = regex_replace(res, reTOD, ss.str());
    }

    return res;
}
    
bool StampString::operator==(const StampString &other) const {
    if (this == &other) {
        return true;
    }
    if (prefix == other.prefix && suffix == other.suffix) {
        return true;
    } else {
        return false;
    }
}

}