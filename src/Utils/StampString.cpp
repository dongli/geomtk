#include "StampString.h"

namespace geomtk {

sregex StampString::re4DigitYear = sregex::compile("%Y");
sregex StampString::re2DigitYear = sregex::compile("%y");
sregex StampString::re2DigitMonth = sregex::compile("%N");
sregex StampString::reMonth = sregex::compile("%m");
sregex StampString::re2DigitDay = sregex::compile("%D");
sregex StampString::reDay = sregex::compile("%d");
sregex StampString::re2DigitHour = sregex::compile("%H");
sregex StampString::re2DigitMinute = sregex::compile("%M");
sregex StampString::re2DigitSecond = sregex::compile("%S");
sregex StampString::re5DigitTOD = sregex::compile("%T");
sregex StampString::reTOD = sregex::compile("%t");
sregex StampString::reStep = sregex::compile("%(\\d)s");

StampString::
StampString() {
    _pattern = "";
}

StampString::
StampString(const string &pattern) {
    _pattern = pattern;
}

StampString::
~StampString() {
}

string StampString::
run(const Time &time) const {
    string res = _pattern;
    smatch what;
    bool matched = false;
    // check year
    if (regex_search(_pattern, what, re4DigitYear)) {
        stringstream ss; ss << time.year;
        res = regex_replace(res, re4DigitYear, ss.str());
        matched = true;
    } else if (regex_search(_pattern, what, re2DigitYear)) {
        stringstream ss; ss << fmod(time.year, 100);
        res = regex_replace(res, re2DigitYear, ss.str());
        matched = true;
    }
    // check month
    if (regex_search(_pattern, what, re2DigitMonth)) {
        stringstream ss; ss << setw(2) << setfill('0') << time.month;
        res = regex_replace(res, re2DigitMonth, ss.str());
        matched = true;
    } else if (regex_search(_pattern, what, reMonth)) {
        stringstream ss; ss << time.month;
        res = regex_replace(res, reMonth, ss.str());
        matched = true;
    }
    // check day
    if (regex_search(_pattern, what, re2DigitDay)) {
        stringstream ss; ss << setw(2) << setfill('0') << time.day;
        res = regex_replace(res, re2DigitDay, ss.str());
        matched = true;
    } else if (regex_search(_pattern, what, reDay)) {
        stringstream ss; ss << time.day;
        res = regex_replace(res, reDay, ss.str());
        matched = true;
    }
    // check TOD
    if (regex_search(_pattern, what, re5DigitTOD)) {
        stringstream ss; ss << setw(5) << setfill('0') << time.tod();
        res = regex_replace(res, re5DigitTOD, ss.str());
        matched = true;
    } else if (regex_search(_pattern, what, reTOD)) {
        stringstream ss; ss << time.tod();
        res = regex_replace(res, reTOD, ss.str());
        matched = true;
    }
    return res;
} // run

string StampString::
run(const TimeManager &timeManager) const {
    string res = _pattern;
    smatch what;
    stringstream ss;
    const Time &time = timeManager.currTime();
    bool matched = false;
    // Check year.
    if (regex_search(_pattern, what, re4DigitYear)) {
        ss.str(""); ss << setw(4) << setfill('0') << time.year;
        res = regex_replace(res, re4DigitYear, ss.str());
        matched = true;
    } else if (regex_search(_pattern, what, re2DigitYear)) {
        ss.str(""); ss << fmod(time.year, 100);
        res = regex_replace(res, re2DigitYear, ss.str());
        matched = true;
    }
    // Check month.
    if (regex_search(_pattern, what, re2DigitMonth)) {
        ss.str(""); ss << setw(2) << setfill('0') << time.month;
        res = regex_replace(res, re2DigitMonth, ss.str());
        matched = true;
    } else if (regex_search(_pattern, what, reMonth)) {
        ss.str(""); ss << time.month;
        res = regex_replace(res, reMonth, ss.str());
        matched = true;
    }
    // Check day.
    if (regex_search(_pattern, what, re2DigitDay)) {
        ss.str(""); ss << setw(2) << setfill('0') << time.day;
        res = regex_replace(res, re2DigitDay, ss.str());
        matched = true;
    } else if (regex_search(_pattern, what, reDay)) {
        ss.str(""); ss << time.day;
        res = regex_replace(res, reDay, ss.str());
        matched = true;
    }
    // Check hour.
    if (regex_search(_pattern, what, re2DigitHour)) {
        ss.str(""); ss << setw(2) << setfill('0') << time.hour;
        res = regex_replace(res, re2DigitHour, ss.str());
        matched = true;
    }
    // Check minute.
    if (regex_search(_pattern, what, re2DigitMinute)) {
        ss.str(""); ss << setw(2) << setfill('0') << time.minute;
        res = regex_replace(res, re2DigitMinute, ss.str());
    }
    // Check second.
    if (regex_search(_pattern, what, re2DigitSecond)) {
        ss.str(""); ss << setw(2) << setfill('0') << time.second;
        res = regex_replace(res, re2DigitSecond, ss.str());
    }
    // Check TOD.
    if (regex_search(_pattern, what, re5DigitTOD)) {
        ss.str(""); ss << setw(5) << setfill('0') << time.tod();
        res = regex_replace(res, re5DigitTOD, ss.str());
        matched = true;
    } else if (regex_search(_pattern, what, reTOD)) {
        ss.str(""); ss << time.tod();
        res = regex_replace(res, reTOD, ss.str());
        matched = true;
    }
    // Check step.
    if (regex_search(_pattern, what, reStep)) {
        int n = stoi(what[1]);
        ss.str(""); ss << setw(n) << setfill('0') << timeManager.numStep();
        res = regex_replace(res, reStep, ss.str());
        matched = true;
    }
    return res;
} // run

string StampString::
wildcard(const string &pattern) {
    string res = pattern;
    smatch what;
    // Check year.
    if (regex_search(pattern, re4DigitYear)) {
        res = regex_replace(res, re4DigitYear, "\\d{4}");
    } else if (regex_search(pattern, re2DigitYear)) {
        res = regex_replace(res, re2DigitYear, "\\d{2}");
    }
    // Check month.
    if (regex_search(pattern, re2DigitMonth)) {
        res = regex_replace(res, re2DigitMonth, "\\d{2}");
    } else if (regex_search(pattern, reMonth)) {
        res = regex_replace(res, reMonth, "\\d+");
    }
    // Check day.
    if (regex_search(pattern, re2DigitDay)) {
        res = regex_replace(res, re2DigitDay, "\\d{2}");
    } else if (regex_search(pattern, reDay)) {
        res = regex_replace(res, reDay, "\\d+");
    }
    // Check hour.
    if (regex_search(pattern, re2DigitHour)) {
        res = regex_replace(res, re2DigitHour, "\\d{2}");
    }
    // Check minute.
    if (regex_search(pattern, re2DigitMinute)) {
        res = regex_replace(res, re2DigitMinute, "\\d{2}");
    }
    // Check second.
    if (regex_search(pattern, re2DigitSecond)) {
        res = regex_replace(res, re2DigitSecond, "\\d{2}");
    }
    // Check TOD.
    if (regex_search(pattern, re5DigitTOD)) {
        res = regex_replace(res, re5DigitTOD, "\\d{5}");
    } else if (regex_search(pattern, reTOD)) {
        res = regex_replace(res, reTOD, "*");
    }
    // Check step.
    if (regex_search(pattern, what, reStep)) {
        int n = stoi(what[1]);
        stringstream ss; ss << "\\d{" << n << "}";
        res = regex_replace(res, reStep, ss.str());
    }
    return res;
} // wildcard

string StampString::
wildcard() const {
    return StampString::wildcard(_pattern);
} // wildcard

bool StampString::
operator==(const StampString &other) const {
    if (this == &other) {
        return true;
    }
    if (_pattern == other._pattern) {
        return true;
    } else {
        return false;
    }
} // operator==

} // geomtk
