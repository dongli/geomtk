#ifndef __GEOMTK_Numerics__
#define __GEOMTK_Numerics__

#include "geomtk_commons.h"

namespace geomtk {

class Numerics {
public:
    template <typename T>
    static vector<T>
    factors(T x);
}; // Numerics

template <typename T>
vector<T> Numerics::
factors(T x) {
    vector<T> res;
    for (unsigned long i = 2; i*i <= x; ++i) {
        while (x%i == 0) {
            res.push_back(i);
            x /= i;
        }
    }
    if (x > 1) res.push_back(x);
    return res;
}

} // geomtk

#endif // __GEOMTK_Numerics__