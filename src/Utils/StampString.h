#ifndef __GEOMTK_StampString__
#define __GEOMTK_StampString__

#include "geomtk_commons.h"
#include "TimeManager.h"

namespace geomtk {

enum StampType {
    TIME_STAMP
};

class StampString {
protected:
    string _pattern;

    static sregex re4DigitYear;
    static sregex re2DigitYear;
    static sregex re2DigitMonth;
    static sregex reMonth;
    static sregex re2DigitDay;
    static sregex reDay;
    static sregex re2DigitHour;
    static sregex re2DigitMinute;
    static sregex re2DigitSecond;
    static sregex re5DigitTOD;
    static sregex reTOD;
    static sregex reStep;
public:
    StampString();
    StampString(const string &pattern);
    virtual ~StampString();

    const string&
    pattern() const {
        return _pattern;
    }

    void
    init(const string &pattern) {
        _pattern = pattern;
    }

    string
    run(const Time &time) const;

    string
    run(const TimeManager &timeManager) const;

    static string
    wildcard(const string &pattern);

    string
    wildcard() const;

    bool
    operator==(const StampString &other) const;
    
    friend ostream&
    operator<<(ostream &os, const StampString &stampString) {
        os << stampString._pattern;
        return os;
    }
}; // StampString

} // geomtk

#endif // __GEOMTK_StampString__
