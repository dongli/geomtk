#include "CartesianDomain.h"

namespace geomtk {

vec CartesianDomain::
diffCoord(const SpaceCoord &x, const SpaceCoord &y) const {
    vec res = x()-y();
    double tmp1, tmp2;
    for (int m = 0; m < numDim(); ++m) {
        if (axisStartBndType(m) == PERIODIC) {
            tmp1 = fabs(res[m]);
            tmp2 = axisSpan(m)-tmp1;
            if (tmp1 > tmp2) {
                res[m] = res[m] > 0 ? -tmp2 : tmp2;
            }
        }
    }
    return res;
}

double CartesianDomain::
calcDistance(const SpaceCoord &x, const SpaceCoord &y) const {
    return norm(diffCoord(x, y));
}

}