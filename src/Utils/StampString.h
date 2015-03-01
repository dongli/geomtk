#ifndef __Geomtk_StampString__
#define __Geomtk_StampString__

#include "geomtk_commons.h"
#include "TimeManager.h"

namespace geomtk {

enum StampType {
    TIME_STAMP
};

class StampString {
protected:
    string pattern;

    static sregex re4DigitYear;
    static sregex re2DigitYear;
    static sregex re2DigitMonth;
    static sregex reMonth;
    static sregex re2DigitDay;
    static sregex reDay;
    static sregex re5DigitTOD;
    static sregex reTOD;
    static sregex reStep;
public:
    StampString();
    StampString(const string &pattern);
    virtual ~StampString();

    void init(const string &pattern) { this->pattern = pattern; }

    string run(const Time &time);
    string run(const TimeManager &timeManager);

    static string wildcard(const string &pattern);
    string wildcard() const;

    bool operator==(const StampString &other) const;
    
    friend ostream& operator<<(ostream &os, const StampString &stampString) {
        os << stampString.pattern;
        return os;
    }
};

}

#endif
