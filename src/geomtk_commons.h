#ifndef __geomtk_commons__
#define __geomtk_commons__

#include <armadillo>
#include <netcdf.h>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <cmath>
#include <string>
#include <vector>

namespace geomtk {

using arma::vec;
using arma::mat;
using arma::cube;
using arma::span;

using std::cout;
using std::endl;
using std::setw;
using std::setprecision;
using std::setfill;
using std::ostream;

using std::string;
using std::vector;
using std::abs;
using std::min;
using std::max;

// -----------------------------------------------------------------------------
// report macros
inline string getFunctionName(const string &str) {
    size_t j = str.find("(");
    assert(j != string::npos);
    size_t i = j-1;
    while (i > 0) {
        if (str.at(i) == ' ') {
            i++;
            break;
        }
        i--;
    }
    return str.substr(i, j-i);
}

#define REPORT_ERROR(MESSAGE) \
{ \
    std::cout << "[Error]: "; \
    std::cout << geomtk::getFunctionName(__PRETTY_FUNCTION__) << ":"; \
    std::cout << __LINE__ << ": "; \
    std::cout << MESSAGE << std::endl; \
    exit(-1); \
}

#define REPORT_NOTICE(MESSAGE) \
{ \
    std::cout << "[Notice]: "; \
    std::cout << geomtk::getFunctionName(__PRETTY_FUNCTION__) << ": "; \
    std::cout << MESSAGE << std::endl; \
}

#define REPORT_WARNING(MESSAGE) \
{ \
    std::cout << "[Warning]: "; \
    std::cout << geomtk::getFunctionName(__PRETTY_FUNCTION__) << ": "; \
    std::cout << MESSAGE << std::endl; \
}

#define CHECK_POINT \
{ \
    std::cout << "[Check point]: " << __FILE__ << ": " << __LINE__ << std::endl; \
}

// -----------------------------------------------------------------------------
// access specifier modifying macros
#ifdef UNIT_TEST
#define private public
#define protected public
#endif

// ------------------------------------------------------------------------------
// constants
const double RAD = M_PI/180.0;
const double PI2 = M_PI*2.0;

}

#endif
