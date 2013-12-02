#ifndef __commons__
#define __commons__

#include <armadillo>

using arma::vec;
using arma::mat;
using arma::cube;
using arma::span;

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;
using std::setw;
using std::setprecision;

#include <assert.h>
#include <cmath>
#include <string>
#include <vector>

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
    cout << "[Error]: "; \
    cout << getFunctionName(__PRETTY_FUNCTION__) << ":"; \
    cout << __LINE__ << ": "; \
    cout << MESSAGE << endl; \
    exit(-1); \
}

#define REPORT_NOTICE(MESSAGE) \
{ \
    cout << "[Notice]: "; \
    cout << getFunctionName(__PRETTY_FUNCTION__) << ": "; \
    cout << MESSAGE << endl; \
}

#define REPORT_WARNING(MESSAGE) \
{ \
    cout << "[Warning]: "; \
    cout << getFunctionName(__PRETTY_FUNCTION__) << ": "; \
    cout << MESSAGE << endl; \
}

#define CHECK_POINT \
{ \
    cout << "[Check point]: " << __FILE__ << ": " << __LINE__ << endl; \
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

#endif
