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

using std::string;

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
    cout << getFunctionName(__PRETTY_FUNCTION__) << ": "; \
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

#endif
