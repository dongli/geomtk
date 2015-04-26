#include "Numerics.h"

namespace geomtk {

vector<uword> Numerics::
factors(uword x) {
    vector<uword> res;
    for (uword i = 2; i*i <= x; ++i) {
        while (x%i == 0) {
            res.push_back(i);
            x /= i;
        }
    }
    if (x > 1) res.push_back(x);
    return res;
} // factors

} // geomtk