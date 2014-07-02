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
    string prefix, suffix;
    string pattern;

    sregex re4DigitYear;
    sregex re2DigitYear;
    sregex re2DigitMonth;
    sregex reMonth;
    sregex re2DigitDay;
    sregex reDay;
    sregex re5DigitTOD;
    sregex reTOD;
public:
    StampString();
    StampString(const string &prefix, const string &suffix);
    StampString(const string &pattern);
    virtual ~StampString();

    void init(const string &pattern) { this->pattern = pattern; }

    string run(const Time &time);
    string run(const string &format, int tag);
    string run(const TimeManager &timeManager);

    bool operator==(const StampString &other) const;
    
    friend ostream& operator<<(ostream &os, const StampString &stampString) {
        os << stampString.prefix << "<...>" << stampString.suffix;
        return os;
    }
};

}

#endif