#ifndef __geomtk_commons__
#define __geomtk_commons__

#include <boost/type_traits/is_same.hpp>
#include <boost/variant.hpp>
#include <boost/any.hpp>
#include <boost/regex.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <netcdf.h>
#include <mlpack/methods/range_search/range_search.hpp>
#include <armadillo>

#include <iostream>
#include <sstream>
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
#include <random>

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
using boost::xpressive::sregex;
using boost::xpressive::smatch;
using boost::xpressive::mark_tag;
using boost::xpressive::repeat;
using boost::xpressive::_d;
using boost::xpressive::_w;

typedef mlpack::tree::BinarySpaceTree<
    mlpack::bound::HRectBound<2>,
    mlpack::range::RangeSearchStat> Tree;
typedef mlpack::metric::EuclideanDistance Metric;
typedef mlpack::range::RangeSearch<Metric, Tree> Searcher;
using mlpack::math::Range;

using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::setprecision;
using std::setfill;
using std::ostream;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::stringstream;
using std::initializer_list;
using std::string;
using std::vector;
using std::list;
using std::map;
using std::min;
using std::max;
using std::getline;

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

// access specifier modifying macros
#ifdef UNIT_TEST
#define private public
#define protected public
#define MUTE
#endif

// print macros
#define REPORT_ERROR(MESSAGE) \
{ \
    std::cerr << "[Error]: "; \
    std::cerr << geomtk::getFunctionName(__PRETTY_FUNCTION__) << ":"; \
    std::cerr << __LINE__ << ": "; \
    std::cerr << MESSAGE << std::endl; \
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
#define REPORT_NOTICE(MESSAGE) \
{ \
}
#endif

#ifndef MUTE
#define REPORT_WARNING(MESSAGE) \
{ \
    std::cerr << "[Warning]: "; \
    std::cerr << geomtk::getFunctionName(__PRETTY_FUNCTION__) << ": "; \
    std::cerr << MESSAGE << std::endl; \
}
#else
#define REPORT_WARNING(MESSAGE) \
{ \
}
#endif

#ifndef MUTE
#define REPORT_ONLINE \
{ \
    std::cout << "[Notice]: "; \
    std::cout << geomtk::getClassName(__PRETTY_FUNCTION__) << " is online."; \
    std::cout << std::endl; \
}
#else
#undef REPORT_ONLINE
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

#define CHECK_NC_OPEN(IERR, FILE_NAME) \
{ \
    if (IERR != NC_NOERR) { \
        REPORT_ERROR("Failed to open file \"" << FILE_NAME << "\" with " << \
                     "error message \"" << nc_strerror(IERR) << "\"!"); \
    } \
}

#define CHECK_NC_CREATE(IERR, FILE_NAME) \
{ \
    if (IERR != NC_NOERR) { \
        REPORT_ERROR("Failed to create file \"" << FILE_NAME << "\" with " << \
                     "error message \"" << nc_strerror(IERR) << "\"!"); \
    } \
}

#define CHECK_NC_CLOSE(IERR, FILE_NAME) \
{ \
    if (IERR != NC_NOERR) { \
        REPORT_ERROR("Failed to close file \"" << FILE_NAME << "\" with " << \
                     "error message \"" << nc_strerror(IERR) << "\"!"); \
    } \
}

#define CHECK_NC_DEF_DIM(IERR, FILE_NAME, DIM_NAME) \
{ \
    if (IERR != NC_NOERR) { \
        REPORT_ERROR("Failed to define dimension \"" << DIM_NAME << "\" with " << \
                     "error message \"" << nc_strerror(IERR) << "\" in file " << \
                     "\"" << FILE_NAME << "\"!"); \
    } \
}

#define CHECK_NC_INQ_DIMID(IERR, FILE_NAME, DIM_NAME) \
{ \
    if (IERR != NC_NOERR) { \
        REPORT_ERROR("Failed to inquire ID of dimension \"" << DIM_NAME << "\" with " << \
                     "error message \"" << nc_strerror(IERR) << "\" in file " << \
                     "\"" << FILE_NAME << "\"!"); \
    } \
}

#define CHECK_NC_INQ_DIMLEN(IERR, FILE_NAME, DIM_NAME) \
{ \
    if (IERR != NC_NOERR) { \
        REPORT_ERROR("Failed to inquire length of dimension \"" << DIM_NAME << "\" with " << \
                     "error message \"" << nc_strerror(IERR) << "\" in file " << \
                     "\"" << FILE_NAME << "\"!"); \
    } \
}

#define CHECK_NC_DEF_VAR(IERR, FILE_NAME, VAR_NAME) \
{ \
    if (IERR != NC_NOERR) { \
        REPORT_ERROR("Failed to define variable \"" << VAR_NAME << "\" with " << \
                     "error message \"" << nc_strerror(IERR) << "\" in file " << \
                     "\"" << FILE_NAME << "\"!"); \
    } \
}

#define CHECK_NC_INQ_VARID(IERR, FILE_NAME, VAR_NAME) \
{ \
    if (IERR != NC_NOERR) { \
        REPORT_ERROR("Failed to inquire ID of variable \"" << VAR_NAME << "\" with " << \
                     "error message \"" << nc_strerror(IERR) << "\" in file " << \
                     "\"" << FILE_NAME << "\"!"); \
    } \
}

#define CHECK_NC_INQ_VARTYPE(IERR, FILE_NAME, VAR_NAME) \
{ \
    if (IERR != NC_NOERR) { \
        REPORT_ERROR("Failed to inquire type of variable \"" << VAR_NAME << "\" with " << \
                     "error message \"" << nc_strerror(IERR) << "\" in file " << \
                     "\"" << FILE_NAME << "\"!"); \
    } \
}

#define CHECK_NC_PUT_VAR(IERR, FILE_NAME, VAR_NAME) \
{ \
    if (IERR != NC_NOERR) { \
        REPORT_ERROR("Failed to put variable \"" << VAR_NAME << "\" with " << \
                     "error message \"" << nc_strerror(IERR) << "\" in file " << \
                     "\"" << FILE_NAME << "\"!"); \
    } \
}

#define CHECK_NC_GET_VAR(IERR, FILE_NAME, VAR_NAME) \
{ \
    if (IERR != NC_NOERR) { \
        REPORT_ERROR("Failed to get variable \"" << VAR_NAME << "\" with " << \
                     "error message \"" << nc_strerror(IERR) << "\" in file " << \
                     "\"" << FILE_NAME << "\"!"); \
    } \
}

#define CHECK_NC_PUT_ATT(IERR, FILE_NAME, VAR_NAME, ATT_NAME) \
{ \
    if (IERR != NC_NOERR) { \
        REPORT_ERROR("Failed to put attribute \"" << ATT_NAME << "\" of " << \
                     "variable \"" << VAR_NAME << "\" with error message \"" << \
                     nc_strerror(IERR) << "\" in file \"" << FILE_NAME << "\"!"); \
    } \
}

#define CHECK_NC_GET_ATT(IERR, FILE_NAME, VAR_NAME, ATT_NAME) \
{ \
    if (IERR != NC_NOERR) { \
        REPORT_ERROR("Failed to get attribute \"" << ATT_NAME << "\" of " << \
                     "variable \"" << VAR_NAME << "\" with error message \"" << \
                     nc_strerror(IERR) << "\" in file \"" << FILE_NAME << "\"!"); \
    } \
}

// constants
const double RAD = M_PI/180.0;
const double PI2 = M_PI*2.0;

}

#endif
