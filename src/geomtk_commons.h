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
#include <type_traits>

namespace geomtk {

using arma::vec;
using arma::mat;
using arma::cube;
using arma::span;
using arma::field;

using std::cout;
using std::endl;
using std::setw;
using std::setprecision;
using std::setfill;
using std::ostream;
using std::ifstream;

using std::string;
using std::vector;
using std::min;
using std::max;
    
// -----------------------------------------------------------------------------
// meta-programming
using std::enable_if;
using std::is_arithmetic;
using std::is_pointer;
using std::is_floating_point;
    
template <typename T, typename NameGetter>
struct has_member_impl {
    typedef char yes;
    typedef long no;

    template <typename C>
    static yes test(typename NameGetter::template get<C>*);

    template <typename C>
    static no  test(...);

    static const bool value = (sizeof(test<T>(0)) == sizeof(yes));
};

template <typename T, typename NameGetter>
struct has_member
    : std::integral_constant<bool, has_member_impl<T, NameGetter>::value> {};

struct check_has_operator_plus {
    template <typename T, T& (T::*)(const T&) = &T::operator+>
    struct get {};
};

template <typename T>
struct has_operator_plus : has_member<T, check_has_operator_plus> {};

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
    
inline string getClassName(const string &str) {
    size_t j = str.rfind("::");
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

#define REPORT_ONLINE \
{ \
    std::cout << "[Notice]: "; \
    std::cout << geomtk::getClassName(__PRETTY_FUNCTION__) << " is online."; \
    std::cout << std::endl; \
}
    
#define REPORT_OFFLINE \
{ \
    std::cout << "[Notice]: "; \
    std::cout << geomtk::getClassName(__PRETTY_FUNCTION__) << " is offline."; \
    std::cout << std::endl; \
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
