#ifndef __geomtk_commons__
#define __geomtk_commons__

#include <armadillo>
#include <netcdf.h>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <cmath>
#include <cstdlib>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cstdarg>
#include <typeinfo>
#include <boost/type_traits/is_same.hpp>
#include <boost/variant.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
//#include <random>

namespace geomtk {

using arma::vec;
using arma::mat;
using arma::cube;
using arma::span;
using arma::field;

using boost::is_same;
using boost::variant;
using boost::regex;
using boost::regex_search;
using boost::match_results;
using boost::regex_replace;

using std::cout;
using std::endl;
using std::setw;
using std::setprecision;
using std::setfill;
using std::ostream;
using std::ifstream;

using std::string;
using std::vector;
using std::list;
using std::map;
using std::min;
using std::max;
using std::getline;

// -----------------------------------------------------------------------------
// report macros
inline string getFunctionName(const string &str) {
    size_t j = str.find("(");
    assert(j != string::npos);
    size_t i = j-1;
    bool isTemplateArguments = false;
    while (i > 0) {
        if (str.at(i) == '>') {
            isTemplateArguments = true;
        } else if (str.at(i) == '<') {
            isTemplateArguments = false;
        }
        if (str.at(i) == ' ' && !isTemplateArguments) {
            i++;
            break;
        }
        i--;
    }
    return str.substr(i, j-i);
}
    
inline string getClassName(const string &str) {
    size_t k = str.rfind("[");
    size_t i, j;
    if (k != string::npos) {
        j = str.substr(0, k).rfind("::");
    } else {
        j = str.rfind("::");
#ifdef DEBUG
        assert(j != string::npos);
#endif
    }
    i = j-1;
    while (i > 0) {
        if (str.at(i) == ' ') {
            i++;
            break;
        }
        i--;
    }
    return str.substr(i, j-i);
}

// -----------------------------------------------------------------------------
// access specifier modifying macros
#ifdef UNIT_TEST
#define private public
#define protected public
#define MUTE
#endif

// -----------------------------------------------------------------------------
// print macros
#define REPORT_ERROR(MESSAGE) \
{ \
    std::cout << "[Error]: "; \
    std::cout << geomtk::getFunctionName(__PRETTY_FUNCTION__) << ":"; \
    std::cout << __LINE__ << ": "; \
    std::cout << MESSAGE << std::endl; \
    exit(-1); \
}

#ifndef MUTE
#define REPORT_NOTICE(MESSAGE) \
{ \
    std::cout << "[Notice]: "; \
    std::cout << geomtk::getFunctionName(__PRETTY_FUNCTION__) << ": "; \
    std::cout << MESSAGE << std::endl; \
}
#else
#define REPORT_NOTICE(MESSAGE)
#endif

#ifndef MUTE
#define REPORT_WARNING(MESSAGE) \
{ \
    std::cout << "[Warning]: "; \
    std::cout << geomtk::getFunctionName(__PRETTY_FUNCTION__) << ": "; \
    std::cout << MESSAGE << std::endl; \
}
#else
#define REPORT_WARNING(MESSAGE)
#endif

#ifndef MUTE
#define REPORT_ONLINE \
{ \
    std::cout << "[Notice]: "; \
    std::cout << geomtk::getClassName(__PRETTY_FUNCTION__) << " is online."; \
    std::cout << std::endl; \
}
#else
#define REPORT_ONLINE
#endif

#ifndef MUTE
#define REPORT_OFFLINE \
{ \
    std::cout << "[Notice]: "; \
    std::cout << geomtk::getClassName(__PRETTY_FUNCTION__) << " is offline."; \
    std::cout << std::endl; \
}
#else
#define REPORT_OFFLINE
#endif

#ifndef MUTE
#define CHECK_POINT \
{ \
    std::cout << "[Check point]: " << __FILE__ << ": " << __LINE__ << std::endl; \
}
#else
#define CHECK_POINT
#endif

// ------------------------------------------------------------------------------
// constants
const double RAD = M_PI/180.0;
const double PI2 = M_PI*2.0;

}

#endif
