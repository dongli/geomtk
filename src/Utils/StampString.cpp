#include "StampString.h"

namespace geomtk {

sregex StampString::re4DigitYear = sregex::compile("%Y");
sregex StampString::re2DigitYear = sregex::compile("%y");
sregex StampString::re2DigitMonth = sregex::compile("%M");
sregex StampString::reMonth = sregex::compile("%m");
sregex StampString::re2DigitDay = sregex::compile("%D");
sregex StampString::reDay = sregex::compile("%d");
sregex StampString::re5DigitTOD = sregex::compile("%T");
sregex StampString::reTOD = sregex::compile("%t");
sregex StampString::reStep = sregex::compile("%(\\d)s");

StampString::StampString() {
    pattern = "";
}

StampString::StampString(const string &pattern) {
    this->pattern = pattern;
}

StampString::~StampString() {
}

string StampString::run(const Time &time) {
    string res = pattern;
    smatch what;
    bool matched = false;
    // check year
    if (regex_search(pattern, what, re4DigitYear)) {
        stringstream ss; ss << time.year;
        res = regex_replace(res, re4DigitYear, ss.str());
        matched = true;
    } else if (regex_search(pattern, what, re2DigitYear)) {
        stringstream ss; ss << fmod(time.year, 100);
        res = regex_replace(res, re2DigitYear, ss.str());
        matched = true;
    }
    // check month
    if (regex_search(pattern, what, re2DigitMonth)) {
        stringstream ss; ss << setw(2) << setfill('0') << time.month;
        res = regex_replace(res, re2DigitMonth, ss.str());
        matched = true;
    } else if (regex_search(pattern, what, reMonth)) {
        stringstream ss; ss << time.month;
        res = regex_replace(res, reMonth, ss.str());
        matched = true;
    }
    // check day
    if (regex_search(pattern, what, re2DigitDay)) {
        stringstream ss; ss << setw(2) << setfill('0') << time.day;
        res = regex_replace(res, re2DigitDay, ss.str());
        matched = true;
    } else if (regex_search(pattern, what, reDay)) {
        stringstream ss; ss << time.day;
        res = regex_replace(res, reDay, ss.str());
        matched = true;
    }
    // check TOD
    if (regex_search(pattern, what, re5DigitTOD)) {
        stringstream ss; ss << setw(5) << setfill('0') << time.getTOD();
        res = regex_replace(res, re5DigitTOD, ss.str());
        matched = true;
    } else if (regex_search(pattern, what, reTOD)) {
        stringstream ss; ss << time.getTOD();
        res = regex_replace(res, reTOD, ss.str());
        matched = true;
    }
    if (!matched) {
        REPORT_ERROR("Stamp string \"" << pattern << "\" does not match anything!");
    }
    return res;
}

// test.gamil.h0.%Y-%M.nc
string StampString::run(const TimeManager &timeManager) {
    string res = pattern;
    smatch what;
    const Time &time = timeManager.getCurrTime();
    bool matched = false;
    // check year
    if (regex_search(pattern, what, re4DigitYear)) {
        stringstream ss; ss << time.year;
        res = regex_replace(res, re4DigitYear, ss.str());
        matched = true;
    } else if (regex_search(pattern, what, re2DigitYear)) {
        stringstream ss; ss << fmod(time.year, 100);
        res = regex_replace(res, re2DigitYear, ss.str());
        matched = true;
    }
    // check month
    if (regex_search(pattern, what, re2DigitMonth)) {
        stringstream ss; ss << setw(2) << setfill('0') << time.month;
        res = regex_replace(res, re2DigitMonth, ss.str());
        matched = true;
    } else if (regex_search(pattern, what, reMonth)) {
        stringstream ss; ss << time.month;
        res = regex_replace(res, reMonth, ss.str());
        matched = true;
    }
    // check day
    if (regex_search(pattern, what, re2DigitDay)) {
        stringstream ss; ss << setw(2) << setfill('0') << time.day;
        res = regex_replace(res, re2DigitDay, ss.str());
        matched = true;
    } else if (regex_search(pattern, what, reDay)) {
        stringstream ss; ss << time.day;
        res = regex_replace(res, reDay, ss.str());
        matched = true;
    }
    // check TOD
    if (regex_search(pattern, what, re5DigitTOD)) {
        stringstream ss; ss << setw(5) << setfill('0') << time.getTOD();
        res = regex_replace(res, re5DigitTOD, ss.str());
        matched = true;
    } else if (regex_search(pattern, what, reTOD)) {
        stringstream ss; ss << time.getTOD();
        res = regex_replace(res, reTOD, ss.str());
        matched = true;
    }
    // check step
    if (regex_search(pattern, what, reStep)) {
        int n = stoi(what[1]);
        stringstream ss; ss << setw(n) << setfill('0') << timeManager.getNumStep();
        res = regex_replace(res, reStep, ss.str());
        matched = true;
    }
    if (!matched) {
        REPORT_ERROR("Stamp string \"" << pattern << "\" does not match anything!");
    }
    return res;
}
    
bool StampString::operator==(const StampString &other) const {
    if (this == &other) {
        return true;
    }
    if (pattern == other.pattern) {
        return true;
    } else {
        return false;
    }
}

}
