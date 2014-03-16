#include "StampString.h"

namespace geomtk {

StampString::StampString() {
    prefix = "";
    suffix = "";
}

StampString::StampString(const string &prefix, const string &suffix) {
    this->prefix = prefix;
    this->suffix = suffix;
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